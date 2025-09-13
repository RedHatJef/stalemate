//
// Created by redha on 9/2/2025.
//

#ifndef STALEMATE_SIMPLEQUEUE_H
#define STALEMATE_SIMPLEQUEUE_H

#include <Arduino.h>

template <typename T, size_t Capacity>
class SimpleQueue {
public:
    SimpleQueue() { clear(); }   // constructor uses clear()

    bool push(const T& item) {
        if (isFull()) return false;
        buffer[tail] = item;
        tail++;
        if(tail >= Capacity) tail = 0;
        count++;
        return true;
    }

    bool pop(T& item) {
        if (isEmpty()) return false;
        item = buffer[head];
        head++;
        if(head >= Capacity) head = 0;
        count--;
        return true;
    }

    bool peek(T& item) const {
        if (isEmpty()) return false;
        item = buffer[head];
        return true;
    }

    void clear() {
        head = 0;
        tail = 0;
        count = 0;
    }

    void printStats(bool printBufContents = false) {
        Serial.printf(F("RXBUF: CAP = %3d, HEAD = %3d, TAIL = %3d, COUNT = %3d\r\n"), Capacity, head, tail, count);
        if(printBufContents) {
            for(int i = 0; i < Capacity; i++) {
                if(i % 4 == 0) Serial.print(' ');
                if(i % 16 == 0) Serial.println();
                Serial.printf(F("%02X "), buffer[i]);
            }
            Serial.println();
            Serial.println();
        }
    }

    [[nodiscard]] bool isEmpty() const { return count == 0; }
    [[nodiscard]] bool isFull()  const { return count == Capacity; }
    [[nodiscard]] uint16_t size()  const { return count; }
    [[nodiscard]] uint16_t capacity() const { return Capacity; }

protected: // allow derived classes to access the ring buffer directly
    T buffer[Capacity];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
};

#endif //STALEMATE_SIMPLEQUEUE_H
