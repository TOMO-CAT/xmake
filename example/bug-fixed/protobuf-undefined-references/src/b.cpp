#include "b.pb.h"
#include <iostream>

const void* a();
int main() {
  std::cout << a() << std::endl;
  std::cout << test::B().descriptor() << std::endl;
  return 0;
}
