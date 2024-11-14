#include <iostream>

#include "proto/test.pb.h"

int main() {
    std::cout << "test protobuf.cpp rule" << std::endl;
    ::github::tomocat::proto::Version version;
    version.set_major_version(1);
    version.set_minor_version(2);
    version.set_patch_version(3);
    version.mutable_e2e_source()->set_module_name("cat");
    return 0;
}
