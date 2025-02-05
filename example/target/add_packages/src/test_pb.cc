#include <google/protobuf/descriptor.h> // 必须包含此头文件
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <iostream>
#include <string>

using namespace google::protobuf;

int main()
{
    std::cout << "Protocol Buffers Test" << std::endl;

    DescriptorPool pool;

    const std::string proto_definition = R"(
        name: "test.proto",
        package: "example",
        dependency: ["A", "B", "C"],
    )";

    FileDescriptorProto file_proto;
    google::protobuf::TextFormat::ParseFromString(proto_definition, &file_proto);

    std::cout << "FileDescriptorProto: " << file_proto.DebugString() << std::endl;
    return 0;
}
