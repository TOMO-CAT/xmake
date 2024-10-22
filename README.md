# README

A cross-platform build utility based on Lua, better suited for large-scale projects.

## build && install

```bash
git submodule update --init
./configure
make
sudo make install PREFIX=/usr/local

xmake --version
```

## test

```bash
# run all tests
bash scripts/test.sh

# run tests with filter
bash scripts/test.sh actions/test
```
