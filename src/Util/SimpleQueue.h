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
        tail = (tail + 1) % Capacity;
        count++;
        return true;
    }

    bool pop(T& item) {
        if (isEmpty()) return false;
        item = buffer[head];
        head = (head + 1) % Capacity;
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

    bool isEmpty() const { return count == 0; }
    bool isFull()  const { return count == Capacity; }
    size_t size()  const { return count; }
    size_t capacity() const { return Capacity; }

protected: // allow derived classes to access the ring buffer directly
    T buffer[Capacity];
    size_t head;
    size_t tail;
    size_t count;
};

#endif //STALEMATE_SIMPLEQUEUE_H
