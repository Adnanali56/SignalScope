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
