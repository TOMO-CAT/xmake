#include <iostream>

int main(int argc, char** argv) {
  char const* arg = argc > 1 ? argv[1] : "xmake";
  std::cout << "hello " << arg << std::endl;
  return -1;
}
