// SearchReport.h
// Stores the outcome of ONE search: algorithm, target, index, comparisons, time.
// Times are recorded in NANOSECONDS everywhere in this program.
#pragma once

#include <iostream>
#include <string>
#include <utility>

class SearchReport {
private:
    std::string algorithmName_;
    std::string target_;        // stored as text so the class need not be a template
    long long   index_;         // -1 means "not found"
    long long   comparisons_;
    long long   timeNs_;        // nanoseconds

public:
    SearchReport()
        : algorithmName_("Unknown"), target_(""), index_(-1),
          comparisons_(0), timeNs_(0) {}

    SearchReport(std::string algorithmName, std::string target,
                 long long index, long long comparisons, long long timeNs)
        : algorithmName_(std::move(algorithmName)), target_(std::move(target)),
          index_(index), comparisons_(comparisons), timeNs_(timeNs) {}

    // Accessors
    const std::string& getAlgorithmName() const { return algorithmName_; }
    const std::string& getTarget() const { return target_; }
    long long getIndex() const { return index_; }
    long long getComparisons() const { return comparisons_; }
    long long getTimeNs() const { return timeNs_; }

    bool found() const { return index_ >= 0; }

    friend std::ostream& operator<<(std::ostream& os, const SearchReport& r) {
        os << "Algorithm: " << r.algorithmName_ << '\n'
           << "Target: " << r.target_ << '\n'
           << "Result: " << (r.found() ? "FOUND" : "NOT FOUND") << '\n'
           << "Index: " << r.index_ << '\n'
           << "Comparisons: " << r.comparisons_ << '\n'
           << "Time: " << r.timeNs_ << " nanoseconds";
        return os;
    }
};
