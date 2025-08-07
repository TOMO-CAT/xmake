# README

## issue

[1] <https://github.com/TOMO-CAT/xmake/issues/220>

## 背景

* [#189](https://github.com/TOMO-CAT/xmake/issues/189): 默认使用 package full linkpath，避免库重名导致的问题

上了这个 issue 之后，如果一个 binary target 依赖一个 object target，那么 binary target 运行时会无法加载动态库。

以当前 example 为例：

```bash
# 修改后 (不能正常运行)
# 报错: error while loading shared libraries: build/linux/x86_64/release/libadd.so: cannot open shared object file: No such file or directory
/usr/bin/clang++ -o build/linux/x86_64/release/main build/.objs/main/linux/x86_64/release/src/main.cc.o -m64 -Wl,-rpath=$ORIGIN build/linux/x86_64/release/libadd.so

# 修改前 (能正常运行)
/usr/bin/clang++ -o build/linux/x86_64/release/main build/.objs/main/linux/x86_64/release/src/main.cc.o -m64 -Lbuild/linux/x86_64/release -Wl,-rpath=$ORIGIN -ladd
```

## 解析

`-Wl,-rpath=$ORIGIN` 表示可执行文件所在的目录，用于指定程序运行时动态库 `.so` 搜索路径在程序运行目录中。
