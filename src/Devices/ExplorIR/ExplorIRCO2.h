// ExplorIRCO2_NonBlocking_NoHeap_Fixed128.h
// AVR128DA32 (DxCore) compatible; no Arduino String; no malloc/new; non-blocking update()

#pragma once
#include <Arduino.h>
#include <math.h>

class ExplorIRCO2 {
public:
    // Measurement mask bits (per datasheet)
    enum Mask : uint16_t {
        MASK_CO2_FILTERED   = 4,     // 'Z'
        MASK_CO2_UNFILTERED = 2,     // 'z'
        MASK_TEMP_FILTERED  = 8,     // 'v'
        MASK_TEMP_UNFILTERED= 128,   // 'V'
        MASK_LED_UNF        = 16,    // 'O'
        MASK_LED_FILT       = 32,    // 'o'
        MASK_SIG_UNF        = 1024,  // 'D'
        MASK_SIG_FILT       = 2048,  // 'd'
        MASK_ZERO_SETPOINT  = 256,   // 'h'
    };

    enum Mode : uint8_t { Sleep=0, Streaming=1, Polling=2 };

    struct QueryResult {
        int32_t Z_ppm = -1;  // filtered ppm
        int32_t z_ppm = -1;  // unfiltered ppm
        int32_t v     = -1;  // temperature (sensor units)
        int32_t V     = -1;
        int32_t O     = -1;
        int32_t o     = -1;
        int32_t D     = -1;
        int32_t d     = -1;
        int32_t h     = -1;
    };

    struct VersionInfo {
        char fwDateTime[32]; // "MMM DD YYYY,HH:MM:SS"
        char fwRev[16];      // e.g., "LP15132"
        char sensorID[16];   // e.g., "528148"
    };

    explicit ExplorIRCO2(HardwareSerial& port = Serial2, uint32_t timeoutMs = 150)
            : _ser(port), _timeout(timeoutMs) {
        _rx[0] = '\0';
        _rxLen = 0;
        _lastByte = -1;
        _scaleCache = -1;
        _hasNew = false;
    }

    void begin(uint32_t baud=9600) { _ser.begin(baud); }

    // ---------------- Non-blocking pump ----------------
    void update() {
        while (_ser.available()) {
            int ch = _ser.read();
            if (ch < 0) break;
            pushRx((char)ch);
            if (ch == '\n' && _lastByte == '\r') {
                // Terminate at CR, drop CRLF
                if (_rxLen >= 2) _rx[_rxLen-2] = '\0';
                normalizeLine(_rx);
                if (_rx[0]) parseLine(_rx);
                _rxLen = 0; // reset for next line
            }
            _lastByte = ch;
        }
    }

    bool hasNewQuery() const { return _hasNew; }

    void latestQuery(QueryResult& out, bool clearNew=true) const {
        out = _latest;
        if (clearNew) ((ExplorIRCO2*)this)->_hasNew = false;
    }

    // For Polling mode; response parsed on next update()
    bool sendQuery() { return writeLine_P(PSTR("Q")); }

    // ---------------- Output mask ----------------
    bool setOutputMask(uint16_t mask) {
        char cmd[20];
        snprintf(cmd, sizeof(cmd), "M %u", (unsigned)mask);
        return cmdSetEchoLead(cmd, 'M');
    }

    // ---------------- Core reads (blocking helpers for setup/diag) ----------------
    int readFilteredRaw()           { return sendAndReadInt_P(PSTR("Z")); }
    int readUnfilteredRaw()         { return sendAndReadInt_P(PSTR("z")); }
    int readFilteredPPM()           { return applyScale(readFilteredRaw()); }
    int readUnfilteredPPM()         { return applyScale(readUnfilteredRaw()); }
    int getScalingFactor()          { return sendAndReadInt_P(PSTR(".")); }

    int applyScale(int raw, int factor = 0) {
        if (raw < 0) return raw;
        if (factor == 0) factor = cachedScale();
        return raw * factor;
    }
    int ppmToSensorUnits(int ppm, int factor = 0) {
        if (factor == 0) factor = cachedScale();
        if (factor <= 0) factor = 10;
        return (ppm + (factor/2)) / factor;
    }

    // ---------------- Modes / filter (blocking setters) ----------------
    bool setMode(Mode m) {
        char cmd[16];
        snprintf(cmd, sizeof(cmd), "K %u", (unsigned)m);
        return cmdSetEchoLead(cmd, 'K');
    }
    bool setDigitalFilter(uint16_t val) {
        char cmd[20];
        snprintf(cmd, sizeof(cmd), "A %u", (unsigned)val);
        return cmdSetEchoLead(cmd, 'A');
    }
    int  getDigitalFilter()         { return sendAndReadInt_P(PSTR("a")); }

    // ---------------- Zeroing & calibration ----------------
    bool zeroPointAdjust_raw(int reported_units, int actual_units) {
        char cmd[32];
        snprintf(cmd, sizeof(cmd), "F %d %d", reported_units, actual_units);
        return cmdSetEchoLead(cmd, 'F');
    }
    bool zeroPointAdjust_ppm(int reported_ppm, int actual_ppm) {
        int sf = cachedScale();
        return zeroPointAdjust_raw(ppmToSensorUnits(reported_ppm, sf),
                                   ppmToSensorUnits(actual_ppm,  sf));
    }
    bool zeroInFreshAir()           { return cmdSetEchoLead_P(PSTR("G"), 'G'); }
    bool zeroInNitrogen()           { return cmdSetEchoLead_P(PSTR("U"), 'U'); }
    bool zeroInKnownGasPPM(int ppm) {
        char cmd[24];
        snprintf(cmd, sizeof(cmd), "X %d", ppmToSensorUnits(ppm));
        return cmdSetEchoLead(cmd, 'X');
    }
    bool setZeroPoint_raw(int units) {
        char cmd[24];
        snprintf(cmd, sizeof(cmd), "u %d", units);
        return cmdSetEchoLead(cmd, 'u');
    }

    // ---------------- Auto-zero ----------------
    bool setAutoZeroIntervals(float initialDays, float intervalDays) {
        char iBuf[12], tBuf[12], cmd[40];
        formatFloat1(initialDays, iBuf, sizeof(iBuf));   // "1.0"
        formatFloat1(intervalDays, tBuf, sizeof(tBuf));  // "8.0"
        snprintf(cmd, sizeof(cmd), "@ %s %s", iBuf, tBuf);
        return cmdSetEchoExact(cmd);
    }
    bool disableAutoZero()          { return cmdSetEchoExact_P(PSTR("@ 0")); }

    bool getAutoZeroConfig(char* out, size_t outLen) {
        if (!out || outLen==0) return false;
        char line[64];
        if (!sendAndReadLine_P(PSTR("@"), line, sizeof(line))) return false;
        strncpy(out, line, outLen-1); out[outLen-1]='\0';
        return true;
    }

    bool setAutoZeroBackgroundPPM(uint16_t ppm) {
        char cmd[16];
        snprintf(cmd, sizeof(cmd), "P 8 %u", (unsigned)(ppm / 256));
        if (!cmdSetEchoLead(cmd, 'P')) return false;
        snprintf(cmd, sizeof(cmd), "P 9 %u", (unsigned)(ppm % 256));
        return cmdSetEchoLead(cmd, 'P');
    }
    bool setFreshAirZeroPPM(uint16_t ppm) {
        char cmd[16];
        snprintf(cmd, sizeof(cmd), "P 10 %u", (unsigned)(ppm / 256));
        if (!cmdSetEchoLead(cmd, 'P')) return false;
        snprintf(cmd, sizeof(cmd), "P 11 %u", (unsigned)(ppm % 256));
        return cmdSetEchoLead(cmd, 'P');
    }

    // ---------------- Pressure compensation ----------------
    bool setCompensationValue(uint16_t value) {
        char cmd[20];
        snprintf(cmd, sizeof(cmd), "S %u", (unsigned)value);
        return cmdSetEchoLead(cmd, 'S');
    }
    int  getCompensationValue()     { return sendAndReadInt_P(PSTR("s")); }

    uint16_t computeCompValueFromAltitudeFt(int altitude_ft) {
        double P = 1013.0 * pow(1.0 - (0.0000225577 * altitude_ft), 5.255877);
        double seaDiff = 1013.0 - P;
        double factor = (seaDiff * 0.14) / 100.0;
        long val = lround(8192.0 + factor * 8192.0);
        if (val < 0) val = 0;
        if (val > 65535) val = 65535;
        return (uint16_t)val;
    }

    double correctedPPM(double ppm, double pressure_mbar) {
        double C1 = ppm, Y;
        if (C1 < 1500.0) {
            Y =  2.6661e-16*pow(C1,4) - 1.1146e-12*pow(C1,3)
                 + 1.7397e-9*pow(C1,2)  - 1.2556e-6*C1 - 9.8754e-4;
        } else {
            Y =  2.811e-38*pow(C1,6) - 9.817e-32*pow(C1,5) + 1.304e-25*pow(C1,4)
                 - 8.126e-20*pow(C1,3) + 2.311e-14*pow(C1,2) - 2.195e-9*C1 - 1.471e-3;
        }
        return C1 / (1.0 + Y * (1013.0 - pressure_mbar));
    }

    // ---------------- Info ----------------
    bool getVersionInfo(VersionInfo& info) {
        char l1[64], l2[64];
        if (!writeLine_P(PSTR("Y"))) return false;
        if (!readLine(l1, sizeof(l1))) return false;
        if (!readLine(l2, sizeof(l2))) return false;
        ltrim(l1); ltrim(l2);
        bool ok = false;
        if (l1[0]=='Y' && l1[1]==',') {
            const char* p1 = strchr(l1+2, ',');
            if (p1) {
                const char* p2 = strchr(p1+1, ',');
                if (p2) {
                    copySpan(info.fwDateTime, sizeof(info.fwDateTime), l1+2, p2);
                    strncpy(info.fwRev, p2+1, sizeof(info.fwRev)-1);
                    info.fwRev[sizeof(info.fwRev)-1] = '\0';
                    ok = true;
                }
            }
        }
        if (l2[0]=='B' && l2[1]==' ') {
            const char* p = l2 + 2;
            char idbuf[16];
            if (readToken(&p, idbuf, sizeof(idbuf))) {
                strncpy(info.sensorID, idbuf, sizeof(info.sensorID)-1);
                info.sensorID[sizeof(info.sensorID)-1] = '\0';
            }
        }
        return ok;
    }

    bool ping() { return readFilteredRaw() >= 0; }

private:
    // ---------- Config ----------
    static constexpr uint16_t RX_CAP = 128;

    // ---------- State ----------
    HardwareSerial& _ser;
    uint32_t _timeout;

    char     _rx[RX_CAP];
    uint16_t _rxLen;
    int      _lastByte;

    QueryResult _latest{};
    volatile bool _hasNew;

    int _scaleCache;

    // ---------- Non-blocking helpers ----------
    void pushRx(char c) {
        if (_rxLen + 1 >= RX_CAP) { // prevent overflow; reset on overrun
            _rxLen = 0;
        }
        _rx[_rxLen++] = c;
        if (_rxLen < RX_CAP) _rx[_rxLen] = '\0';
    }

    static void normalizeLine(char* s) {
        if (!s) return;
        // rstrip
        size_t n = strlen(s);
        while (n>0 && (s[n-1]=='\r'||s[n-1]=='\n'||s[n-1]==' '||s[n-1]=='\t')) s[--n]=0;
        // ltrim
        size_t i=0,j=0;
        while (s[i]==' '||s[i]=='\t'||s[i]=='\r'||s[i]=='\n') ++i;
        if (i) { while ((s[j++] = s[i++])) {} }
    }

    void parseLine(const char* line) {
        // Token pairs like: " Z 00040 z 00041 ..."
        QueryResult tmp = _latest;

        const char* p = line;
        while (*p) {
            skipSpaces(&p);
            if (!*p) break;
            char key = *p++;
            skipSpaces(&p);
            char valbuf[12];
            if (!readToken(&p, valbuf, sizeof(valbuf))) break;
            int v = atoi(valbuf);

            switch (key) {
                case 'Z': tmp.Z_ppm = applyScale(v); break;
                case 'z': tmp.z_ppm = applyScale(v); break;
                case 'v': tmp.v = v; break;
                case 'V': tmp.V = v; break;
                case 'O': tmp.O = v; break;
                case 'o': tmp.o = v; break;
                case 'D': tmp.D = v; break;
                case 'd': tmp.d = v; break;
                case 'h': tmp.h = v; break;
                default:  /* ignore */ break;
            }
        }

        _latest = tmp;
        _hasNew = true;
    }

    // ---------- Blocking helpers (setup/cal) ----------
    int cachedScale() {
        if (_scaleCache <= 0) {
            _scaleCache = getScalingFactor();
            if (_scaleCache <= 0) _scaleCache = 10;
        }
        return _scaleCache;
    }

    static void ltrim(char* s) {
        if (!s) return;
        size_t i=0, j=0;
        while (s[i] && (s[i]==' '||s[i]=='\r'||s[i]=='\n'||s[i]=='\t')) ++i;
        if (i) { while ((s[j++] = s[i++])) {} }
    }
    static void rstrip(char* s) {
        if (!s) return;
        size_t n = strlen(s);
        while (n>0 && (s[n-1]=='\r'||s[n-1]=='\n'||s[n-1]==' '||s[n-1]=='\t')) { s[--n]=0; }
    }
    static void skipSpaces(const char** pp) {
        const char* p = *pp; while (*p==' '||*p=='\t'||*p=='\r'||*p=='\n') ++p; *pp=p;
    }
    static bool readToken(const char** pp, char* out, size_t outLen) {
        if (!pp||!*pp||!out||outLen==0) return false;
        const char* p=*pp; size_t k=0;
        while (*p && *p!=' ' && *p!='\t' && *p!='\r' && *p!='\n') {
            if (k+1 < outLen) out[k++] = *p;
            ++p;
        }
        out[k]='\0'; *pp=p; return k>0;
    }
    static void copySpan(char* dst, size_t dstLen, const char* beg, const char* endExclusive) {
        size_t n = (size_t)(endExclusive - beg); if (n >= dstLen) n = dstLen - 1;
        memcpy(dst, beg, n); dst[n]='\0';
    }
    static void formatFloat1(float val, char* out, size_t outLen) {
        if (!out || outLen < 4) { if (outLen) out[0]='\0'; return; }
        char tmp[16];
        dtostrf(val, 0, 1, tmp); // width=0 avoids left padding
        // strip any spaces
        size_t i=0, j=0; while (tmp[i]) { if (tmp[i] != ' ') tmp[j++] = tmp[i]; ++i; } tmp[j]='\0';
        strncpy(out, tmp, outLen-1); out[outLen-1]='\0';
    }

    bool writeLine(const char* s) {
        if (!s) return false; _ser.print(s); _ser.print("\r\n"); _ser.flush(); return true;
    }
    bool writeLine_P(PGM_P ps) {
        if (!ps) return false; char c; while ((c=pgm_read_byte(ps++))) _ser.write(c);
        _ser.print("\r\n"); _ser.flush(); return true;
    }
    bool readLine(char* out, size_t outLen) {
        if (!out || outLen==0) return false;
        size_t k=0; uint32_t start=millis(); int last=-1;
        while (millis()-start < _timeout) {
            while (_ser.available()) {
                int ch=_ser.read(); if (ch<0) break;
                if (k+1<outLen) out[k++]=(char)ch;
                if (ch=='\n' && last=='\r') {
                    if (k>=2) { out[k-2]='\0'; rstrip(out); ltrim(out); return true; }
                }
                last=ch;
            }
        }
        if (k>=outLen) k=outLen-1; out[k]='\0'; rstrip(out); ltrim(out);
        return k>0;
    }
    bool sendAndReadLine_P(PGM_P cmd, char* line, size_t lineLen) {
        if (!writeLine_P(cmd)) return false; return readLine(line, lineLen);
    }
    bool sendAndReadLine(const char* cmd, char* line, size_t lineLen) {
        if (!writeLine(cmd)) return false; return readLine(line, lineLen);
    }
    int sendAndReadInt_P(PGM_P cmd) {
        char line[48]; if (!sendAndReadLine_P(cmd, line, sizeof(line))) return -1;
        const char* p=strchr(line,' '); const char* num=p?(p+1):line; while (*num==' ') ++num; if (!*num) return -1;
        return atoi(num);
    }
    bool cmdSetEchoLead(const char* cmd, char expectLead) {
        char line[48]; if (!sendAndReadLine(cmd, line, sizeof(line))) return false; return (line[0]==expectLead);
    }
    bool cmdSetEchoLead_P(PGM_P cmd, char expectLead) {
        char line[48]; if (!sendAndReadLine_P(cmd, line, sizeof(line))) return false; return (line[0]==expectLead);
    }
    bool cmdSetEchoExact(const char* cmd) {
        char line[64]; if (!sendAndReadLine(cmd, line, sizeof(line))) return false; return strcmp(line, cmd)==0;
    }
    bool cmdSetEchoExact_P(PGM_P cmd) {
        char expect[40]; size_t i=0; char c; while ((c=pgm_read_byte(cmd++)) && i+1<sizeof(expect)) expect[i++]=c; expect[i]='\0';
        char line[64]; if (!sendAndReadLine(expect, line, sizeof(line))) return false; return strcmp(line, expect)==0;
    }
};

/* ===========================
   Example usage (Polling, non-blocking)
   ===========================
#include "ExplorIRCO2_NonBlocking_NoHeap_Fixed128.h"

ExplorIRCO2 co2(Serial2);

void setup() {
  Serial.begin(115200);
  co2.begin(9600);

  if (!co2.ping()) {
    Serial.println(F("ExplorIR not responding"));
    return;
  }

  co2.setMode(ExplorIRCO2::Polling); // quiet until asked
  co2.setOutputMask(ExplorIRCO2::MASK_CO2_FILTERED | ExplorIRCO2::MASK_CO2_UNFILTERED);
}

uint32_t tLast=0;
void loop() {
  co2.update(); // always pump to assemble/parse lines

  // poll 2 Hz; response parsed in update()
  if (millis() - tLast > 500) {
    tLast = millis();
    co2.sendQuery();
  }

  if (co2.hasNewQuery()) {
    ExplorIRCO2::QueryResult q;
    co2.latestQuery(q); // clears "new" flag
    if (q.Z_ppm >= 0) {
      Serial.print(F("CO2 (filtered): ")); Serial.println(q.Z_ppm);
    }
  }

  // ... rest of your loop ...
}
*/