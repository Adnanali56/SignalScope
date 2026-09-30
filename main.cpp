// main.cpp - SignalScope: Template-Based Search Analyzer
// Menu-driven front end. No global variables; every feature is its own function.
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "SearchAlgorithms.h"
#include "SearchReport.h"
#include "SignalBuffer.h"

namespace {

// Thrown when stdin is closed (Ctrl+D / end of piped input). Deliberately NOT
// derived from std::exception so the menu's error handler does not swallow it.
struct InputClosed {};

enum class MenuResult { Exit, SwitchType };
enum class Algorithm { Linear, Binary, Ternary, Interpolation };

constexpr long long MAX_CAPACITY = 10000000;

// ------------------------------ input helpers ------------------------------
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw InputClosed{};
    return line;
}

// Re-prompts until the whole line is one valid value of type V.
template <typename V>
V readValue(const std::string& prompt) {
    while (true) {
        std::istringstream in(readLine(prompt));
        V value{};
        if ((in >> value) && (in >> std::ws).eof()) return value;
        std::cout << "Invalid input. Please enter a valid number.\n";
    }
}

// ------------------------------ display helpers ----------------------------
template <typename T>
void printMenu(const SignalBuffer<T>& b, const std::string& typeName) {
    std::cout << "\n=================================================\n"
              << "       SIGNALSCOPE - SEARCH ANALYZER\n"
              << "=================================================\n\n"
              << "Data Type: " << typeName << '\n'
              << "Elements: " << b.size() << '\n'
              << "Capacity: " << b.capacity() << '\n'
              << "Status: " << (b.isSorted() ? "SORTED" : "UNSORTED") << "\n\n"
              << " 1. Create/Reset Dataset\n"
              << " 2. Generate Random Readings\n"
              << " 3. Append Reading\n"
              << " 4. Display Dataset\n"
              << " 5. Sort Dataset\n"
              << " 6. Linear Search\n"
              << " 7. Binary Search\n"
              << " 8. Ternary Search\n"
              << " 9. Interpolation Search\n"
              << "10. Compare All Searches\n"
              << "11. Test Copy Operation\n"
              << "12. Switch Data Type\n"
              << " 0. Exit\n\n";
}

// ------------------------------ menu actions -------------------------------
template <typename T>
void createDataset(SignalBuffer<T>& buffer) {
    const long long cap = readValue<long long>("Capacity for new dataset: ");
    if (cap < 0) { std::cout << "Error: dataset size cannot be negative.\n"; return; }
    if (cap == 0) { std::cout << "Error: capacity must be at least 1.\n"; return; }
    if (cap > MAX_CAPACITY) {
        std::cout << "Error: capacity may not exceed " << MAX_CAPACITY << ".\n";
        return;
    }
    buffer = SignalBuffer<T>(static_cast<std::size_t>(cap));
    std::cout << "New empty dataset created (capacity " << cap << ").\n";
}

template <typename T>
void generateReadings(SignalBuffer<T>& buffer) {
    const long long count = readValue<long long>("Number of readings: ");
    if (count < 0) { std::cout << "Error: number of readings cannot be negative.\n"; return; }
    const T minValue = readValue<T>("Minimum value: ");
    const T maxValue = readValue<T>("Maximum value: ");
    const long long seed = readValue<long long>("Random seed: ");
    if (seed < 0) { std::cout << "Error: seed cannot be negative.\n"; return; }

    buffer.generateRandom(static_cast<std::size_t>(count), minValue, maxValue,
                          static_cast<unsigned int>(seed));
    std::cout << "\n" << count << " readings generated.\n";
}

template <typename T>
void appendReading(SignalBuffer<T>& buffer) {
    const T value = readValue<T>("Enter reading: ");
    buffer += value;
    std::cout << "Reading added.\n";
}

template <typename T>
void displayDataset(const SignalBuffer<T>& buffer) {
    std::cout << "\nDataset:\n" << buffer << "\n\n"
              << "Dataset Status: " << (buffer.isSorted() ? "SORTED" : "UNSORTED") << '\n';
}

template <typename T>
void sortDataset(SignalBuffer<T>& buffer) {
    if (buffer.isEmpty()) { std::cout << "Dataset is empty. Nothing to sort.\n"; return; }
    buffer.sort();
    std::cout << "Dataset sorted successfully.\n\n" << "Dataset:\n" << buffer << '\n';
}

template <typename T>
void runSingleSearch(const SignalBuffer<T>& buffer, Algorithm algo) {
    const T target = readValue<T>("Enter target: ");
    std::cout << '\n';
    SearchReport report;
    switch (algo) {
        case Algorithm::Linear:        report = linearSearch(buffer, target); break;
        case Algorithm::Binary:        report = binarySearch(buffer, target); break;
        case Algorithm::Ternary:       report = ternarySearch(buffer, target); break;
        case Algorithm::Interpolation: report = interpolationSearch(buffer, target); break;
    }
    std::cout << report << '\n';
}

template <typename T>
void compareAllSearches(const SignalBuffer<T>& buffer) {
    if (buffer.isEmpty()) {
        std::cout << "Error: cannot search an empty dataset.\n";
        return;
    }
    if (!buffer.isSorted()) {
        std::cout << "Compare All Searches requires sorted data.\n"
                     "Please sort the dataset first (option 5).\n";
        return;
    }
    const T target = readValue<T>("Enter target: ");

    // Same target, same dataset, all four algorithms.
    const SearchReport reports[4] = {
        linearSearch(buffer, target),
        binarySearch(buffer, target),
        ternarySearch(buffer, target),
        interpolationSearch(buffer, target)};
    const char* labels[4] = {"Linear", "Binary", "Ternary", "Interpolation"};

    const std::string bar(57, '=');
    const std::string dash(57, '-');
    std::cout << '\n' << bar << '\n'
              << "                SEARCH COMPARISON REPORT\n"
              << bar << '\n'
              << "Target: " << target << "\n\n"
              << std::left << std::setw(16) << "Algorithm"
              << std::setw(10) << "Index"
              << std::setw(14) << "Comparisons"
              << "Time\n" << dash << '\n';
    for (int i = 0; i < 4; ++i) {
        std::cout << std::left << std::setw(16) << labels[i]
                  << std::setw(10) << reports[i].getIndex()
                  << std::setw(14) << reports[i].getComparisons()
                  << reports[i].getTimeNs() << " ns\n";
    }
    std::cout << dash << '\n';
}

template <typename T>
void testCopy(const SignalBuffer<T>& original) {
    if (original.isEmpty()) {
        std::cout << "Dataset is empty. Add or generate readings first.\n";
        return;
    }
    const std::size_t mid = original.size() / 2;
    const T savedValue = original[mid];
    const T newValue = (savedValue == T(99)) ? T(77) : T(99);

    // --- copy constructor ---
    SignalBuffer<T> copy(original);
    copy[mid] = newValue;

    // --- copy assignment ---
    SignalBuffer<T> assigned(1);
    assigned = original;
    assigned[mid] = newValue;

    std::cout << "\nOriginal:\n" << original << "\n\n"
              << "Copy (copy constructor) after modification:\n" << copy << "\n\n"
              << "Copy (assignment operator) after modification:\n" << assigned << "\n\n"
              << "Original after copies were modified:\n" << original << "\n\n";

    const bool ok = original[mid] == savedValue && copy[mid] == newValue &&
                    assigned[mid] == newValue;
    std::cout << (ok ? "Deep copy verified." : "ERROR: shallow copy detected!") << '\n';
}

// ------------------------------ menu loop ----------------------------------
template <typename T>
MenuResult runMenu(SignalBuffer<T>& buffer, const std::string& typeName) {
    while (true) {
        printMenu(buffer, typeName);
        const int choice = readValue<int>("Enter selection: ");
        try {
            switch (choice) {
                case 0:  return MenuResult::Exit;
                case 1:  createDataset(buffer); break;
                case 2:  generateReadings(buffer); break;
                case 3:  appendReading(buffer); break;
                case 4:  displayDataset(buffer); break;
                case 5:  sortDataset(buffer); break;
                case 6:  runSingleSearch(buffer, Algorithm::Linear); break;
                case 7:  runSingleSearch(buffer, Algorithm::Binary); break;
                case 8:  runSingleSearch(buffer, Algorithm::Ternary); break;
                case 9:  runSingleSearch(buffer, Algorithm::Interpolation); break;
                case 10: compareAllSearches(buffer); break;
                case 11: testCopy(buffer); break;
                case 12: return MenuResult::SwitchType;
                default: std::cout << "Invalid selection. Please choose 0-12.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "\nError: " << e.what() << '\n';
        }
    }
}

}  // namespace

int main() {
    try {
        SignalBuffer<int> intBuffer(50);
        SignalBuffer<double> doubleBuffer(50);
        bool useInt = true;

        while (true) {
            const MenuResult r = useInt ? runMenu(intBuffer, "INTEGER")
                                        : runMenu(doubleBuffer, "DOUBLE");
            if (r == MenuResult::Exit) break;
            useInt = !useInt;
            std::cout << "\nSwitched to " << (useInt ? "INTEGER" : "DOUBLE") << " data.\n";
        }
        std::cout << "\nGoodbye from SignalScope!\n";
    } catch (const InputClosed&) {
        std::cout << "\n\nInput closed. Exiting SignalScope.\n";
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
