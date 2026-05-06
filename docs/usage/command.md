# 命令

## Create

可以创建临时项目：

```bash
xmake create ${ProjectName}
```

## Build

```bash
# 编译
xmake b -vD

# dry run
xmake b --dry-run
```

## Update

更新 xmake 到指定版本，支持 git tag、git branch、git commit，也支持指定 runtime（luajit 或 lua）。

```bash
# 调试, 只打印升级信息
xmake update --dry-run

# 更新到最新 git tag 版本 (release 版本)
xmake update

# 更新到 master 分支
xmake update master

# 更新到指定 git tag
xmake update v3.0.8

# 更新到指定 git branch
xmake update dev

# 更新到指定 git commit
xmake update a1b2c3d

# 指定 runtime 为 lua (默认是 luajit)
xmake update --runtime=lua master

# 指定 runtime 为 luajit
xmake update --runtime=luajit v3.0.8

# 强制重新安装当前版本
xmake update -f

# 仅更新脚本 (不重新编译)
# -s 表示 --scriptonly
xmake update -s master

# 指定源更新
xmake update -v github:TOMO-CAT/xmake#master
xmake update -v gitee:tomocat/xmake#dev
```
