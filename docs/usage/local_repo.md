# Local Repo

## 简介

有时候出于调试或者本地开发的需求，我们需要将 remote package 替换成 local package 来编译自己的项目。

## 细节

### 1. xmake build 前先 xmake f

由于本地库的特殊性，有一些 bug 场景是因为我们没有在 xmake build 前先运行 xmake f 导致的（当前的实现会在 xmake f 中 install package）。
