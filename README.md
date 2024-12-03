# README

🌟 A cross-platform build utility based on Lua, better suited for large-scale projects.

## build && install

```bash
# 全量安装
bash scripts/install.sh

# 仅更新 Lua 脚本
./scripts/get.sh __local__ __install_only__
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
