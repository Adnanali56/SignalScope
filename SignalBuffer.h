// SignalBuffer.h
// Template class that owns a dynamically allocated array of readings.
#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>

template <typename T>
class SignalBuffer {
    static_assert(std::is_arithmetic_v<T>, "SignalBuffer<T> requires a numeric type.");

private:
    T*          data_;       // dynamically allocated array
    std::size_t size_;       // current number of elements
    std::size_t capacity_;   // maximum number of elements
    bool        sorted_;     // true if data_[0..size_) is in ascending order

public:
    // Creates an empty buffer with the given capacity.
    explicit SignalBuffer(std::size_t capacity = 50)
        : data_(nullptr), size_(0), capacity_(capacity), sorted_(true) {
        if (capacity == 0)
            throw std::invalid_argument("Capacity must be greater than zero.");
        data_ = new T[capacity_]();
    }

    // Queries
    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    bool isEmpty() const { return size_ == 0; }
    bool isFull() const { return size_ == capacity_; }
    bool isSorted() const { return sorted_; }

    // Adds one reading; refuses (throws) instead of writing past the array.
    void append(const T& value) {
        if (isFull())
            throw std::overflow_error("Buffer is full (capacity " +
                                      std::to_string(capacity_) + "). Reading not added.");
        if (size_ > 0 && value < data_[size_ - 1]) sorted_ = false;
        data_[size_++] = value;
    }

    // Sorts ascending (std::sort is allowed for sorting).
    void sort() {
        std::sort(data_, data_ + size_);
        sorted_ = true;
    }
};
