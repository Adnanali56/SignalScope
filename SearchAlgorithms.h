// SearchAlgorithms.h
// Hand-written template search functions. Each returns a SearchReport.
// A "comparison" = one comparison between the target and an array element.
// All algorithms report the LOWEST index when duplicates exist.
// Times are measured with std::chrono around the search itself only (ns).
#pragma once

#include <chrono>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "SearchReport.h"
#include "SignalBuffer.h"

namespace detail {

using Clock = std::chrono::steady_clock;

template <typename T>
std::string toText(const T& value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

template <typename T>
void requireData(const SignalBuffer<T>& buffer, const std::string& name) {
    if (buffer.isEmpty())
        throw std::runtime_error(name + " cannot run on an empty dataset.");
}

template <typename T>
void requireSorted(const SignalBuffer<T>& buffer, const std::string& name) {
    requireData(buffer, name);
    if (!buffer.isSorted())
        throw std::logic_error(name + " requires sorted data.\n"
                               "Please sort the dataset before using this search.");
}

inline long long elapsedNs(Clock::time_point start) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start).count();
}

}  // namespace detail

// ---------------------------------------------------------------------
// 1. Linear Search - works on sorted or unsorted data.
// ---------------------------------------------------------------------
template <typename T>
SearchReport linearSearch(const SignalBuffer<T>& buffer, const T& target) {
    const std::string name = "Linear Search";
    detail::requireData(buffer, name);

    long long comparisons = 0;
    long long index = -1;

    const auto start = detail::Clock::now();
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        ++comparisons;
        if (buffer[i] == target) {       // first hit = lowest index
            index = static_cast<long long>(i);
            break;
        }
    }
    const long long ns = detail::elapsedNs(start);

    return SearchReport(name, detail::toText(target), index, comparisons, ns);
}

// ---------------------------------------------------------------------
// 2. Binary Search - sorted data only. "Leftmost" variant: keeps halving
//    until the first position whose value is >= target is isolated.
// ---------------------------------------------------------------------
template <typename T>
SearchReport binarySearch(const SignalBuffer<T>& buffer, const T& target) {
    const std::string name = "Binary Search";
    detail::requireSorted(buffer, name);

    long long comparisons = 0;
    long long index = -1;
    const std::size_t n = buffer.size();

    const auto start = detail::Clock::now();
    std::size_t lo = 0, hi = n;                 // answer lies in [lo, hi]
    while (lo < hi) {
        const std::size_t mid = lo + (hi - lo) / 2;
        ++comparisons;
        if (buffer[mid] < target) lo = mid + 1;
        else                      hi = mid;
    }
    if (lo < n) {
        ++comparisons;
        if (buffer[lo] == target) index = static_cast<long long>(lo);
    }
    const long long ns = detail::elapsedNs(start);

    return SearchReport(name, detail::toText(target), index, comparisons, ns);
}

// ---------------------------------------------------------------------
// 3. Ternary Search - sorted data only. Two probes (m1, m2) split the
//    region into three sections. Leftmost variant, same idea as above.
// ---------------------------------------------------------------------
template <typename T>
SearchReport ternarySearch(const SignalBuffer<T>& buffer, const T& target) {
    const std::string name = "Ternary Search";
    detail::requireSorted(buffer, name);

    long long comparisons = 0;
    long long index = -1;
    const std::size_t n = buffer.size();

    const auto start = detail::Clock::now();
    std::size_t lo = 0, hi = n;                 // answer lies in [lo, hi]
    while (lo < hi) {
        const std::size_t len = hi - lo;
        const std::size_t m1 = lo + len / 3;
        const std::size_t m2 = lo + (2 * len) / 3;

        ++comparisons;
        if (!(buffer[m1] < target)) {           // buffer[m1] >= target -> first section
            hi = m1;
        } else {
            ++comparisons;
            if (!(buffer[m2] < target)) {       // middle section
                lo = m1 + 1;
                hi = m2;
            } else {                            // last section
                lo = m2 + 1;
            }
        }
    }
    if (lo < n) {
        ++comparisons;
        if (buffer[lo] == target) index = static_cast<long long>(lo);
    }
    const long long ns = detail::elapsedNs(start);

    return SearchReport(name, detail::toText(target), index, comparisons, ns);
}
