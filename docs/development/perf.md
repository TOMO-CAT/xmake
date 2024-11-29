# perf 性能调优

## 安装 perf

安装对应的依赖：

```bash
sudo apt install flex bison systemtap-sdt-dev libssl-dev libslang2-dev libperl-dev liblzma-dev libcap-dev libnuma-dev libbabeltrace-ctf-dev binutils libelf1 python3 libunwind8 zstd 
```

下载源码：

> [上海交大镜像](https://ftp.sjtu.edu.cn/sites/ftp.kernel.org/pub/linux/kernel/)
> [北京交大镜像](https://mirror.bjtu.edu.cn/kernel/linux/kernel/tools/perf/)
> [Linux 官网](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/perf)

查看 Linux 版本：

```bash
$ uname -r
4.15.0-156-generic
```

编译 perf：

```bash
cd /tmp
# wget https://ftp.sjtu.edu.cn/sites/ftp.kernel.org/pub/linux/kernel/v4.x/linux-4.15.1.tar.gz
# tar -zxvf linux-4.15.1.tar.gz
# cd linux-4.15.1/tools/perf/

wget https://mirror.bjtu.edu.cn/kernel/linux/kernel/tools/perf/v4.19.0/perf-4.19.0.tar.gz
tar -zxvf perf-4.19.0.tar.gz
cd perf-4.19.0/tools/perf

unset CPLUS_INCLUDE_PATH
unset C_INCLUDE_PATH
unset CPP
unset CC
unset CXX
unset LD
make -j10 WERROR=0
sudo make install WERROR=0

./perf --version
```
