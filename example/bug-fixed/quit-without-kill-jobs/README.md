# README

## 背景

> <https://github.com/TOMO-CAT/xmake/issues/216>

如果上次编译失败或者未正常退出，有一些后台进程还在占用 project 锁。

考虑以下几种做法：

* 等后台进程编译完再退出（仿造 cmake）
* 直接 kill 没跑完的进程然后退出
* 把文件锁删了（不妥，虽然实现简单，这样如果短时间内多次重试可能会导致机器 OOM）
