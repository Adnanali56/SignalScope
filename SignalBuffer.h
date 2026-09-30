// SignalBuffer.h
// Template class that owns a dynamically allocated array of readings.
// Implements the Rule of Three (destructor, copy ctor, copy assignment).
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
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

    static constexpr std::size_t DISPLAY_LIMIT = 25;

public:
    // ---- Construction / Rule of Three ---------------------------------
    explicit SignalBuffer(std::size_t capacity = 50)
        : data_(nullptr), size_(0), capacity_(capacity), sorted_(true) {
        if (capacity == 0)
            throw std::invalid_argument("Capacity must be greater than zero.");
        data_ = new T[capacity_]();
    }

    // Copy constructor (deep copy)
    SignalBuffer(const SignalBuffer& other)
        : data_(new T[other.capacity_]()), size_(other.size_),
          capacity_(other.capacity_), sorted_(other.sorted_) {
        std::copy(other.data_, other.data_ + other.size_, data_);
    }

    // Copy assignment (deep copy, safe for self-assignment)
    SignalBuffer& operator=(const SignalBuffer& other) {
        if (this != &other) {
            T* newData = new T[other.capacity_]();       // allocate first
            std::copy(other.data_, other.data_ + other.size_, newData);
            delete[] data_;                              // then release old
            data_     = newData;
            size_     = other.size_;
            capacity_ = other.capacity_;
            sorted_   = other.sorted_;
        }
        return *this;
    }

    // Destructor
    ~SignalBuffer() { delete[] data_; }

    // ---- Queries -------------------------------------------------------
    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    bool isEmpty() const { return size_ == 0; }
    bool isFull() const { return size_ == capacity_; }
    bool isSorted() const { return sorted_; }

    // ---- Modifiers -----------------------------------------------------
    void append(const T& value) {
        if (isFull())
            throw std::overflow_error("Buffer is full (capacity " +
                                      std::to_string(capacity_) + "). Reading not added.");
        // Stays sorted only if the new value is >= the current last value.
        if (size_ > 0 && value < data_[size_ - 1]) sorted_ = false;
        data_[size_++] = value;
    }

    void clear() {
        size_ = 0;
        sorted_ = true;
    }

    void sort() {
        std::sort(data_, data_ + size_);
        sorted_ = true;
    }

    // Appends 'count' random readings in [minValue, maxValue]; repeatable via seed.
    void generateRandom(std::size_t count, T minValue, T maxValue, unsigned int seed) {
        if (minValue > maxValue)
            throw std::invalid_argument("Minimum value cannot be greater than maximum value.");
        if (count > capacity_ - size_)
            throw std::overflow_error("Capacity exceeded: only " +
                                      std::to_string(capacity_ - size_) +
                                      " free slot(s) remain.");
        std::mt19937 engine(seed);
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> dist(minValue, maxValue);
            for (std::size_t i = 0; i < count; ++i) append(dist(engine));
        } else {
            std::uniform_real_distribution<T> dist(minValue, maxValue);
            for (std::size_t i = 0; i < count; ++i) {
                T v = static_cast<T>(std::round(dist(engine) * 100.0) / 100.0);
                append(std::clamp(v, minValue, maxValue));
            }
        }
    }

    // ---- Operators -----------------------------------------------------
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

    SignalBuffer& operator+=(const T& value) {
        append(value);
        return *this;
    }

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
