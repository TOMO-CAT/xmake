# README

🌟 A Linux C++ build utility based on Lua, better suited for large-scale projects.

## Install

```bash
bash scripts/install.sh --runtime=luajit
```

## Docker dev environment

```bash
# default: ubuntu_2204
# python3 scripts/docker.py --command clear
python3 scripts/docker.py --command build
python3 scripts/docker.py --command run

# ubuntu_latest
# python3 scripts/docker.py --arch ubuntu_latest --command clear
python3 scripts/docker.py --arch ubuntu_latest --command build
python3 scripts/docker.py --arch ubuntu_latest --command run
```

## Run xmake tests && examples

```bash
# run all tests
bash scripts/test.sh

# run all examples
bash scripts/example.sh
```
