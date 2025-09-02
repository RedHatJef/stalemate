//
// Created by redha on 9/2/2025.
//

#ifndef STALEMATE_EXPLORIRQUEUE_H
#define STALEMATE_EXPLORIRQUEUE_H

#include "Util/SimpleQueue.h"

class ExplorIRQueue : public SimpleQueue<char, 256> {
public:
    ExplorIRQueue() : SimpleQueue<char, 256>() {}

    // Scans for a CRLF-terminated message. If found:
    // - Copies the message (without CRLF) into outBuf, up to outSize-1, and null-terminates.
    // - Removes the message + CRLF from the queue.
    // Returns true if a full message was found and popped.
    bool popMessage(char* outBuf, size_t outSize) {
        if (!outBuf || outSize == 0) return false;
        if (count < 2) return false; // need at least CRLF

        int endOffset = -1; // index of '\r' relative to head
        for (size_t i = 0; i + 1 < count; ++i) {
            size_t idx  = (head + i) % capacity();
            size_t idx2 = (idx + 1) % capacity();
            if (buffer[idx] == '\r' && buffer[idx2] == '\n') {
                endOffset = static_cast<int>(i);
                break;
            }
        }
        if (endOffset < 0) {
            return false; // no full message yet
        }

        size_t msgLen = static_cast<size_t>(endOffset); // length before CRLF

        // Copy message into outBuf (truncate if needed), and null-terminate
        size_t toCopy = (msgLen < (outSize - 1)) ? msgLen : (outSize - 1);
        for (size_t j = 0; j < toCopy; ++j) {
            size_t idx = (head + j) % capacity();
            outBuf[j] = buffer[idx];
        }
        outBuf[toCopy] = '\0';

        // Consume message + CRLF from the queue
        size_t consume = msgLen + 2; // +2 for '\r\n'
        head = (head + consume) % capacity();
        count -= consume;

        return true;
    }
};

#endif //STALEMATE_EXPLORIRQUEUE_H
