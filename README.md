# README

🌟 A Linux C++ build utility based on Lua, better suited for large-scale projects.

## build && install

```bash
# install xmake && it's scripts (luajit)
bash scripts/install.sh --runtime=luajit

# install xmake && it's scripts (lua 5.4.6)
bash scripts/install.sh

# only install xmake scripts
# ./scripts/get.sh __local__ __install_only__
```

## test

```bash
# run all tests
bash scripts/test.sh

# run tests with filter
bash scripts/test.sh actions/test

# run specific test
xmake l tests/modules/scheduler/runjobs.lua

# run all examples
bash scripts/example.sh
```

## development

```bash
xmake -P core -bvD --all
```
