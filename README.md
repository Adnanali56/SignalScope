# SignalScope: Template-Based Search Analyzer

A menu-driven C++17 program for Data Structures III (Lab 1). It stores numeric
readings and searches them with four algorithms, counting comparisons and
timing each search.

## Links
- YouTube demo: _add link_
- LinkedIn post: _add link_

## What it does
- Stores `int` or `double` readings in one template class, `SignalBuffer<T>`
- Generates random readings from a seed, so results can be repeated
- Searches with **Linear**, **Binary**, **Ternary** and **Interpolation** search
- Shows the index found, number of comparisons and time (nanoseconds)
- Compares all four searches on the same data

## Search algorithms
| Algorithm | Sorted data needed? |
|---|---|
| Linear | No |
| Binary | Yes |
| Ternary | Yes |
| Interpolation | Yes |

If a value appears more than once, the lowest index is reported.

## How templates are used
- `SignalBuffer<T>` is a template class, used as `SignalBuffer<int>` and `SignalBuffer<double>`
- The four search functions are template functions (`linearSearch<T>`, etc.)
- There are no separate int/double copies of any code

## Files
- `SignalBuffer.h` - template class (Rule of Three, `[]`, `+=`, `<<`)
- `SearchReport.h` - result of one search
- `SearchAlgorithms.h` - the four template search functions
- `main.cpp` - the menu
- `UML_Diagram.pdf` - class diagram
- `sample_output.txt` - example run

## How to compile and run
```
g++ -std=c++17 -o signalscope main.cpp
./signalscope
```
On Windows use `signalscope.exe`.

## How to use
1. Choose **2** to generate random readings (count, min, max, seed).
2. Choose **4** to display the data.
3. Choose **5** to sort. Binary, Ternary and Interpolation need sorted data.
4. Choose **6-9** to run a search, or **10** to compare all four.
5. Choose **11** to test the deep copy, **12** to switch between int and double.
6. Choose **0** to exit.
