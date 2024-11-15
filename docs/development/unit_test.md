# Unit Test

## 环境准备

安装 Lua：

```bash
# 安装 Lua 5.3
# sudo apt install lua5.3

# 安装 Lua 5.4
cd /tmp
wget http://www.lua.org/ftp/lua-5.4.4.tar.gz
tar -zxvf lua-5.4.4.tar.gz
cd lua-5.4.4/
make
sudo make install

# 查看版本
lua -v
```

安装 luarocks 包管理工具：

```bash
sudo apt-get install luarocks
```

安装 busted 单测工具：

```bash
# root
luarocks install busted

# 非 root
luarocks install busted --local
export PATH=$PATH:~/.luarocks/bin
```

## 运行单测

```bash
busted xmake
```

## 运行所有的 example

```bash
bash scripts/example.sh
```

## 运行所有的 example busted tests

```bash
bash scripts/busted.sh
```
