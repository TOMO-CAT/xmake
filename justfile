# xmake 开发任务（需要 https://github.com/casey/just ）
# 在仓库根目录执行；会设置与 scripts/*.sh 相同的环境（如 XMAKE_PROGRAM_DIR）。

set shell := ["bash", "-c"]

# 默认列出可用命令
[private]
default:
	@just --list

# 运行 example 下所有 test.sh（同 scripts/example.sh）
example:
	bash scripts/example.sh

# 运行 tests/run.lua（同 scripts/test.sh）
# 用法: just test
#       just test projects/c++/snippet_runtimes
#       just test tests/apis/clone_target
test *ARGS:
	bash scripts/test.sh {{ARGS}}

# 进入 docker 开发容器（复用 scripts/docker.py）
# 用法: just docker                 # 默认进入 ubuntu_latest
#       just docker ubuntu_2204
#       just docker ubuntu_latest
docker ARCH="ubuntu_latest":
	python3 scripts/docker.py --arch {{ARCH}} --command build
	python3 scripts/docker.py --arch {{ARCH}} --command run

# 清理 docker 开发容器和镜像（复用 scripts/docker.py）
# 用法: just docker-clear                 # 默认清理 ubuntu_latest
#       just docker-clear ubuntu_2204
#       just docker-clear ubuntu_latest
docker-clear ARCH="ubuntu_latest":
	python3 scripts/docker.py --arch {{ARCH}} --command clear
