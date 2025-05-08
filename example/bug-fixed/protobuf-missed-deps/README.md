# README

## issue

<https://github.com/TOMO-CAT/xmake/issues/194>

## 问题描述

如果 a.proto 中引用了 b.proto, a.pb.cc 会等待 a.proto 编译完成, 但是不会等待 b.proto 编译完成。这意味着编译 a.pb.cc 的时候, b.pb.h 可能还没有生成, 导致编译失败。
