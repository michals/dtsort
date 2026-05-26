#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// Define adaptivity configuration:
//   0 = High Entropy Mode (optimal for fully random inputs, pure decision tree)
//   1 = Adaptive Mode (optimal for sorted, reversed, or runs, standard default)
#ifndef DT_ADAPTIVE
#define DT_ADAPTIVE 1
#endif

// Include the dtsort interface
#include "dt_stable_sort.hh"

struct Student {
  std::string name;
  int grade;

  // Helper to print
  friend std::ostream &operator<<(std::ostream &os, const Student &s) {
    return os << s.name << " (" << s.grade << ")";
  }
};

int main() {
  std::cout << "========================================\n";
  std::cout << "  dtsort: Small-Array Sorting Demo\n";
  std::cout << "  Adaptivity Mode (DT_ADAPTIVE): " << DT_ADAPTIVE << "\n";
  std::cout << "========================================\n\n";

  // -------------------------------------------------------------------------
  // Demo 1: Sorting integers (size 8)
  // -------------------------------------------------------------------------
  std::vector<int> numbers = {42, 17, 88, 3, 9, 54, 11, 29};
  std::cout << "Demo 1: Sorting basic integers (N = 8)\n";
  std::cout << "  Original: ";
  for (int n : numbers)
    std::cout << n << " ";
  std::cout << "\n";

  // Perform optimal small-array stable sort
  dt_stable_sort(numbers.begin(), numbers.end());

  std::cout << "  Sorted:   ";
  for (int n : numbers)
    std::cout << n << " ";
  std::cout << "\n\n";

  // -------------------------------------------------------------------------
  // Demo 2: Stable-sorting custom structs with custom lambda comparator (size
  // 5)
  // -------------------------------------------------------------------------
  std::vector<Student> students = {
      {"Alice", 90},
      {"Bob", 85},
      {"Charlie",
       90}, // Grade matches Alice, should remain after Alice (stability)
      {"Dave", 70},
      {"Eva", 85} // Grade matches Bob, should remain after Bob
  };

  std::cout << "Demo 2: Stable-sorting custom objects with lambda (N = 5)\n";
  std::cout << "  Original:\n";
  for (const auto &s : students)
    std::cout << "    " << s << "\n";

  // Sort descending by grade to showcase custom comparator and stability
  dt_stable_sort(
      students.begin(), students.end(),
      [](const Student &a, const Student &b) { return a.grade > b.grade; });

  std::cout << "  Sorted by grade (descending):\n";
  for (const auto &s : students)
    std::cout << "    " << s << "\n";
  std::cout << "\n";

  // Quick verification of sorting correctness
  bool is_sorted = std::is_sorted(
      students.begin(), students.end(),
      [](const Student &a, const Student &b) { return a.grade > b.grade; });

  if (is_sorted) {
    std::cout << "Success: All elements sorted correctly!\n";
  } else {
    std::cout << "Error: Elements not sorted correctly.\n";
  }

  return 0;
}
