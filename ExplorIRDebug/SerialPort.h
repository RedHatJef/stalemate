// SerialPort.h
#pragma once
#define NOMINMAX
#include <windows.h>
#include <string>
#include <cstdint>

class SerialPort {
public:
    SerialPort() = default;
    ~SerialPort() { close(); }

    // Non-copyable, but movable
    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;
    SerialPort(SerialPort&& other) noexcept { *this = std::move(other); }
    SerialPort& operator=(SerialPort&& other) noexcept {
        if (this != &other) {
            close();
            h_        = other.h_;
            is_open_  = other.is_open_;
            other.h_ = INVALID_HANDLE_VALUE;
            other.is_open_ = false;
        }
        return *this;
    }

    // Open with defaults: COM5, 115200-8-N-1
    bool open(const std::wstring& portName = L"COM6",
              DWORD baud = CBR_115200,
              BYTE  byteSize = 8,
              BYTE  parity   = NOPARITY,
              BYTE  stopBits = ONESTOPBIT)
    {
        close();

        std::wstring path = normalize_port_name(portName);

        h_ = CreateFileW(path.c_str(),
                         GENERIC_READ | GENERIC_WRITE,
                         0,            // exclusive access
                         nullptr,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         nullptr);

        if (h_ == INVALID_HANDLE_VALUE)
            return false;

        // Set I/O buffer sizes (optional but recommended)
        if (!SetupComm(h_, 1 << 14, 1 << 14)) { // 16KB in/out
            close();
            return false;
        }

        // Configure timeouts for non-blocking reads:
        // Read returns immediately if no data available.
        COMMTIMEOUTS timeouts{};
        timeouts.ReadIntervalTimeout         = MAXDWORD;
        timeouts.ReadTotalTimeoutMultiplier  = 0;
        timeouts.ReadTotalTimeoutConstant    = 0;
        timeouts.WriteTotalTimeoutMultiplier = 0;
        timeouts.WriteTotalTimeoutConstant   = 0;
        if (!SetCommTimeouts(h_, &timeouts)) {
            close();
            return false;
        }

        // Configure line settings
        DCB dcb{};
        dcb.DCBlength = sizeof(DCB);
        if (!GetCommState(h_, &dcb)) {
            close();
            return false;
        }

        dcb.BaudRate = baud;
        dcb.ByteSize = byteSize;
        dcb.Parity   = parity;
        dcb.StopBits = stopBits;

        dcb.fBinary  = TRUE;
        dcb.fParity  = (parity != NOPARITY);
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fDtrControl  = DTR_CONTROL_ENABLE; // assert DTR
        dcb.fDsrSensitivity = FALSE;
        dcb.fTXContinueOnXoff = TRUE;
        dcb.fOutX = FALSE;
        dcb.fInX  = FALSE;
        dcb.fErrorChar = FALSE;
        dcb.fNull = FALSE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;  // assert RTS
        dcb.fAbortOnError = FALSE;

        if (!SetCommState(h_, &dcb)) {
            close();
            return false;
        }

        // Clear any junk
        PurgeComm(h_, PURGE_RXCLEAR | PURGE_TXCLEAR);
        is_open_ = true;
        return true;
    }

    void close() {
        if (h_ != INVALID_HANDLE_VALUE) {
            CloseHandle(h_);
            h_ = INVALID_HANDLE_VALUE;
        }
        is_open_ = false;
    }

    bool isOpen() const { return is_open_; }

    // Returns number of bytes currently queued in RX buffer
    DWORD available() const {
        if (!is_open_) return 0;
        DWORD errors = 0;
        COMSTAT stat{};
        if (!ClearCommError(h_, &errors, &stat)) return 0;
        return stat.cbInQue;
    }

    // Non-blocking: returns -1 if no char available
    int readChar() {
        if (!is_open_) return -1;
        if (available() == 0) return -1;

        char c = 0;
        DWORD read = 0;
        if (!ReadFile(h_, &c, 1, &read, nullptr))
            return -1;
        return (read == 1) ? static_cast<unsigned char>(c) : -1;
    }

    // Reads up to maxLen bytes currently available; returns count
    DWORD read(void* buf, DWORD maxLen) {
        if (!is_open_ || maxLen == 0) return 0;
        DWORD toRead = available();
        if (toRead == 0) return 0;
        if (toRead > maxLen) toRead = maxLen;

        DWORD read = 0;
        if (!ReadFile(h_, buf, toRead, &read, nullptr))
            return 0;
        return read;
    }

    // Write a single character
    bool writeChar(char c) {
        if (!is_open_) return false;
        DWORD written = 0;
        return WriteFile(h_, &c, 1, &written, nullptr) && written == 1;
    }

    // Write a buffer; returns bytes written
    DWORD write(const void* buf, DWORD len) {
        if (!is_open_ || len == 0) return 0;
        DWORD written = 0;
        if (!WriteFile(h_, buf, len, &written, nullptr))
            return 0;
        return written;
    }

    // Convenience: write a C-string (without the null terminator)
    DWORD writeString(const char* s) {
        if (!s) return 0;
        DWORD len = static_cast<DWORD>(strlen(s));
        return write(s, len);
    }

    // Optional: control lines
    void setDTR(bool enable) {
        if (!is_open_) return;
        EscapeCommFunction(h_, enable ? SETDTR : CLRDTR);
    }
    void setRTS(bool enable) {
        if (!is_open_) return;
        EscapeCommFunction(h_, enable ? SETRTS : CLRRTS);
    }

private:
    static std::wstring normalize_port_name(const std::wstring& name) {
        // Always use \\.\COMx format (works for COM10+ too)
        if (name.rfind(L"\\\\.\\", 0) == 0) return name;
        return L"\\\\.\\" + name;
    }

    HANDLE h_ = INVALID_HANDLE_VALUE;
    bool   is_open_ = false;
};
