FROM ubuntu:22.04

RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list \
    && apt-get clean && apt-get update && apt-get install -y --fix-missing \
    build-essential \
    git \
    iputils-ping \
    bear \
    wget \
    sudo \
    curl \
    cmake \
    make \
    clang \
    libc++abi-dev libc++-dev \
    unzip \
    ccache \
    file \
    htop \
    binutils-dev \
    python3-distutils \
    lcov \


# packages/2502/l/llvm/14.0.0/source/llvm/tools/gold/gold-plugin.cpp:38:10: fatal error: plugin-api.h: No such file or directory
# sudo apt install binutils-dev
# @see https://blog.51cto.com/u_13721254/6163639

# ModuleNotFoundError: No module named 'distutils.spawn'
# sudo apt install python3-distutils -y
# @see https://github.com/nodejs/node/issues/30189

# tests/projects/c++/snippet_runtimes: 'iostream' file not found
# sudo apt-get install libc++abi-dev libc++-dev
# @see https://stackoverflow.com/questions/55390435/iostream-file-not-found-when-compiling-on-wsl/55393044
# @see https://stackoverflow.com/questions/58364711/xcode-11-1-iostream-file-not-found

# Could NOT find OpenGL
# sudo apt-get install libgl1-mesa-dev
# @see https://stackoverflow.com/questions/31170869/cmake-could-not-find-opengl-in-ubuntu
