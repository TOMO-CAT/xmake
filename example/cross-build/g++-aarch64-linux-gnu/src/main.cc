#include <iostream>

#include "header.pb.h"

int main(int argc, char** argv) {
    std::cout << "hello world!" << std::endl;
    ::github::tomocat::proto::End2EndSource e2e_src;
    e2e_src.set_module_name("cat");
    e2e_src.set_sequence_num(10);
    e2e_src.set_transit_time_ms(1.1);
    return 0;
}
