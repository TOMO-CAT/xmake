# README

## 问题描述

当 `protobuf.cpp` rule 的 proto 文件启用 `proto_grpc_cpp_plugin` 时，rule 会把
`.grpc.pb.cc.o` 加入目标对象列表，但旧实现没有实际编译这个对象文件。依赖该
proto target 的最终链接因此会失败：

```text
ld: cannot find .../service.grpc.pb.cc.o
```

这个例子使用一个 object proto target 和一个依赖它的 binary target，复现
kratos 项目中的目标结构。

## 运行

需要系统提供 `protoc`、`grpc_cpp_plugin`、protobuf C++ 和 gRPC C++ 开发库：

```sh
xmake b -v
```
