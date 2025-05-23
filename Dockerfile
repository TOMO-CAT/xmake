FROM ubuntu:22.04

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
    gdb \
    vim

# install xmake master
ENV XMAKE_ROOT=y
ENV XMAKE_STATS=n
ENV XMAKE_PROGRAM_DIR=/usr/local/share/xmake

RUN mkdir /software && cd /software \
    && git clone --progress -v --recursive https://gitee.com/tomocat/xmake.git \
    && cd xmake \
    && bash scripts/install.sh \
    && xmake --version \
    && cd / && rm -r software

# pre-commit lua 格式化依赖 luarocks
RUN apt-get install -y \
    python3-pip \
    luarocks

RUN cat <<'EOF' >> /etc/bash.bashrc

# add proxy
export hostip="host.docker.internal"
export socks_hostport=7890
export http_hostport=7890
alias proxy='
    export https_proxy="http://${hostip}:${http_hostport}"
    export http_proxy="http://${hostip}:${http_hostport}"
    export ALL_PROXY="socks5://${hostip}:${socks_hostport}"
    export all_proxy="socks5://${hostip}:${socks_hostport}"
'
alias unproxy='
    unset ALL_PROXY
    unset https_proxy
    unset http_proxy
    unset all_proxy
'
alias echoproxy='
    echo $ALL_PROXY
    echo $all_proxy
    echo $https_proxy
    echo $http_proxy
'
#end proxy
EOF

ARG USER_NAME
RUN useradd -m ${USER_NAME}
RUN echo "root:0000" | chpasswd
RUN echo "${USER_NAME}:0000" | chpasswd
RUN echo "${USER_NAME} ALL=NOPASSWD: ALL" >> /etc/sudoers
USER ${USER_NAME}

# pre-commit
RUN python3 -m pip install pre-commit==3.5.0

ENV PATH=${PATH}:/home/${USER_NAME}/.local/bin
