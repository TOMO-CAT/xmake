# README

## 简介

测试用的 example。

## 设置

为方便开发，将源码路径添加到 xmake programdir 中，这样运行的就是当前开发的 xmake 源码脚本：

```bash
# 设置 xmake 脚本目录为源码目录, 方便开发时调试
export XMAKE_PROGRAM_DIR="${PWD}/xmake"

# 此时查看 programdir 已经变化
$ xmake show
The information of xmake:
    version: 3.0.1+dev.51131a198
    host: linux/x86_64
    programdir: /home/cat/Documents/github/xmake/xmake
    programfile: /usr/local/bin/xmake
    globaldir: /home/cat/.xmake
    tmpdir: /tmp/.xmake1000/241111
    workingdir: /home/cat/Documents/github/xmake
    packagedir: /home/cat/.xmake/packages
    packagedir(cache): /home/cat/.xmake/cache/packages/2411
    gitsource: github.com
```

## 调试命令

```bash
xrepo env -b emmylua_debugger -v -- xmake b -vrD
```
