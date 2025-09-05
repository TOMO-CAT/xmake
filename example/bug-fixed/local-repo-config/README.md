# README

## 背景

> <https://github.com/TOMO-CAT/xmake/issues/229>
假设 xsim 依赖 xperc, 在 xsim 里编译 xperc 依赖库时，会发现 xsim 设置了 `-jn` 参数无法传递给 xperc，从而导致在机器资源不足时 OOM。

另外经过测试，`--rebuild` 也不会传递。
