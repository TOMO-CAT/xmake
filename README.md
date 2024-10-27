# README

🌟 A cross-platform build utility based on Lua, better suited for large-scale projects.

## build && install

```bash
bash scripts/install.sh
```

## test

```bash
# run all tests
bash scripts/test.sh

# run tests with filter
bash scripts/test.sh actions/test

# run specific test
xmake l tests/modules/scheduler/runjobs.lua
```

## development

```bash
xmake -P core -bvD --all
```
