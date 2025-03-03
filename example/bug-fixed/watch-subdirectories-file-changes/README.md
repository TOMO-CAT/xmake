# README

## issue

[https://github.com/xmake-io/xmake/issues/5645](https://github.com/xmake-io/xmake/issues/5645)

对于 Linux 系统而言，xmake watch 命令无法递归检测子目录中的文件变动。

## 用法

```bash
xmake watch -d "watch_test" -c 'xmake run test'
# or
xmake watch -t test
```

然后每次修改 `watch/dir1/dir2/test.cc` 文件就会触发编译。
