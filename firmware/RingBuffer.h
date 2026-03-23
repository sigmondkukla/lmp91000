/*
 * RingBuffer.h
 *
 *  Created on: Dec 23, 2025
 *      Author: Sigmond
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <cstdint>
#include <cstddef>

template <typename T, size_t Size>
class RingBuffer {
public:
    RingBuffer() : read_index(0), write_index(0), is_full(false) {}

    bool push(const T& item) {
        if (is_full) {
            return false; // better check this and do something about it
        }

        buffer[write_index] = item;
        write_index = (write_index + 1) % Size;

        if (write_index == read_index) {
            is_full = true;
        }

        return true;
    }

    bool pop(T& item) {
        if (isEmpty()) {
            return false; // also should check this, but it isn't a bad thing
        }

        item = buffer[read_index];
        read_index = (read_index + 1) % Size;
        is_full = false;

        return true;
    }

    size_t count() const {
        if (is_full) return Size;
        if (write_index >= read_index) return write_index - read_index;
        return Size + write_index - read_index;
    }

    bool isEmpty() const {
        return (!is_full && (write_index == read_index));
    }

    bool isFull() const {
        return is_full;
    }

    void clear() {
        read_index = 0;
        write_index = 0;
        is_full = false;
    }

private:
    T buffer[Size];
    volatile size_t read_index;
    volatile size_t write_index;
    volatile bool is_full;
};

#endif /* RINGBUFFER_H_ */
