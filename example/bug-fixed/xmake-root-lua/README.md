# README

## 背景

如果在一个 xmake 项目存放在另一个 xmake 项目中，当前 xmake 逻辑会从当前目录一直往外找到最外层的 xmake.lua 作为项目根目录的 xmake.lua。这可能会导致查找到错误的根目录从而识别不到当前项目的 target，所以我们仿造 BLADE 构造 xmake_root.lua。
