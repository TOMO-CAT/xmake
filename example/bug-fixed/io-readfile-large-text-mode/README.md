# io.readfile 文本模式读取大文件出现尾部清零 / 字节错乱

## 背景

调用 `io.open(path, "r"):read("*a")`（默认 text mode）读取大于 ~128KB 的纯文本文件时，
返回的字符串长度正确，但其中靠近 128KB 边界处的若干字节被替换成 `\0`，
导致后续解析（例如把内容当成 Lua 源码 `loadstring`）失败：

```bash
error: ./xmake.lua:324: '}' expected near 'zgpu'
```

实际触发场景：

- 子进程通过 `XMAKE_RCFILES` 把数百 KB 的 `add_requireconfs(...)` rcfile 内容
  prepend 到工程 `xmake.lua` 解析时；
- `string.serialize` / `io.readfile` 等把超过 128KB 的内容拼接 / 解析时。

## 根因

C 层 `xu_native_large_allocator_ralloc`（`core/src/xutil/memory/impl/native_large_allocator.c`）
在 `xu_buffer_resize` 跨越 mmap / 普通堆分界（`XU_VIRTUAL_MEMORY_DATA_MINN = 128KB`）做
small ↔ large 迁移时，`memcpy` 长度算成了 `base_head->size`，
源指针又是块的元数据头 `data_head`，导致用户数据末尾少拷贝了 `sizeof(xu_native_large_data_head_t)`
（24 bytes，正好两个指针 + 一个 size_t）的内容。
新的 mmap 区域出厂全 0，所以表现为：尾部 24 字节范围内的有效用户数据被替换成了 `\0`。

## 复现

```bash
bash test.sh
```

未修复时（即使用未修复的 xmake 二进制）：

- `test.lua` 会生成一个 ~140KB 的纯文本文件；
- 在 text 模式下 `read("*a")`，校验回读内容与写入内容相同；
- 旧版本会在 ~130KB 处拷出一段 `\0` 字节（具体 offset / 宽度依不同行长有差异），
  脚本以非零退出码报错：`FAIL: text-mode read corrupted at offset ...`。

修复后，`test.sh` 退出码为 0，并打印 `PASS: ...`.
