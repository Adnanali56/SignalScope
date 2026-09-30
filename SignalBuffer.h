// SignalBuffer.h
// Template class that owns a dynamically allocated array of readings.
#pragma once

#include <algorithm>
#include <cstddef>
#include <iostream>
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

    static constexpr std::size_t DISPLAY_LIMIT = 25;   // max values printed by <<

public:
    // Creates an empty buffer with the given capacity.
    explicit SignalBuffer(std::size_t capacity = 50)
        : data_(nullptr), size_(0), capacity_(capacity), sorted_(true) {
        if (capacity == 0)
            throw std::invalid_argument("Capacity must be greater than zero.");
        data_ = new T[capacity_]();
    }

    // ---- Rule of Three ----------------------------------------------

    // 1. Copy constructor: allocates its OWN array and copies the values (deep copy).
    SignalBuffer(const SignalBuffer& other)
        : data_(new T[other.capacity_]()), size_(other.size_),
          capacity_(other.capacity_), sorted_(other.sorted_) {
        std::copy(other.data_, other.data_ + other.size_, data_);
    }

    // 2. Copy assignment: deep copy, safe for self-assignment (a = a).
    SignalBuffer& operator=(const SignalBuffer& other) {
        if (this != &other) {
            T* newData = new T[other.capacity_]();   // allocate first
            std::copy(other.data_, other.data_ + other.size_, newData);
            delete[] data_;                          // then release the old array
            data_     = newData;
            size_     = other.size_;
            capacity_ = other.capacity_;
            sorted_   = other.sorted_;
        }
        return *this;
    }

    // 3. Destructor: releases the dynamic array.
    ~SignalBuffer() { delete[] data_; }

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

    // ---- Operators ----------------------------------------------------

    // Read-only access with bounds checking.
    const T& operator[](std::size_t index) const {
        if (index >= size_)
            throw std::out_of_range("Invalid index " + std::to_string(index) +
                                    " (size is " + std::to_string(size_) + ").");
        return data_[index];
    }

    // Writable access with bounds checking. The caller may change the value,
    // so the buffer conservatively marks itself UNSORTED.
    T& operator[](std::size_t index) {
        if (index >= size_)
            throw std::out_of_range("Invalid index " + std::to_string(index) +
                                    " (size is " + std::to_string(size_) + ").");
        sorted_ = false;
        return data_[index];
    }

    // buffer += value;  appends one reading.
    SignalBuffer& operator+=(const T& value) {
        append(value);
        return *this;
    }

    // cout << buffer;  prints at most 25 values.
    friend std::ostream& operator<<(std::ostream& os, const SignalBuffer& b) {
        if (b.size_ == 0) return os << "(empty)";
        const std::size_t shown = std::min(b.size_, DISPLAY_LIMIT);
        for (std::size_t i = 0; i < shown; ++i) {
            if (i > 0) os << ' ';
            os << b.data_[i];
        }
        if (b.size_ > DISPLAY_LIMIT) os << "\n... additional values not shown";
        return os;
    }
};
