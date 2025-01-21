# README

## 开发

```bash
git clone --recursive https://github.com/xmake-io/xmake.git
cd core

# 调试 xutil
cd src/xutil
xmake build -P . -v

# 运行 xutil 单测
cd src/xutil
xmake config --ut=y -P . -v -c
xmake build -P . -v --all
```

## Embed

将 xmake 及其依赖的 lua 脚本编译成二进制：

```bash
cd core
xmake f --embed=y
xmake build
```

## arm

```bash
# 拉取镜像
docker run --rm --privileged multiarch/qemu-user-static:register --reset
# arm
docker pull multiarch/ubuntu-core:armhf-bionic
# arm64
docker pull multiarch/ubuntu-core:arm64-bionic

# 启动容器
# arm
docker run -v "${HOME}/github/xmake:/root/xmake"  --name ubuntu_arm -it multiarch/ubuntu-core:armhf-bionic
# arm64
docker run -v "${HOME}/github/xmake:/root/xmake"  --name ubuntu_arm64 -it multiarch/ubuntu-core:arm64-bionic

# 安装依赖
apt update
apt install -y make build-essential
```
