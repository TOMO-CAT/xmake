#include <iostream>

#include "src/1920x1080.h"

template <const int64_t* arr, size_t N>
struct ArraySum {
    static const int64_t value = arr[N - 1] + ArraySum<arr, N - 1>::value;
};

template <const int64_t* arr>
struct ArraySum<arr, 0> {
    static const int64_t value = 0;
};

template <const int64_t* arr, size_t N>
struct ArrayMaximum {
    static const int64_t value = std::max(arr[N - 1], ArrayMaximum<arr, N - 1>::value);
};

template <const int64_t* arr>
struct ArrayMaximum<arr, 0> {
    static const int64_t value = 0;
};

struct Cat4 {
  Cat4() { std::cout << "Cat4" << std::endl; }
  ~Cat4() = default;
};

Cat4 cat4;

void helper4() {
  const size_t size = sizeof(k1920x1080IndexMapY) / sizeof(k1920x1080IndexMapY[0]);
  const int64_t sum = ArraySum<k1920x1080IndexMapY, size>::value;
  const int64_t max = ArrayMaximum<k1920x1080IndexMapY, size>::value;

  std::cout << sum << std::endl;
  std::cout << max << std::endl;
}
