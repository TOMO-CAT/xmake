#include <iostream>
#include "perc/perc.h"

int main() {
    int sum = xmake::perc::add(10, 20);
    std::cout << "sum: " << sum << std::endl;
}
