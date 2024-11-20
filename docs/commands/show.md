# Show

## Basic Information

```bash
$ xmake show
The information of xmake:
    version: 2.9.3+dev.7b1707203
    host: linux/x86_64
    programdir: /usr/local/share/xmake
    programfile: /usr/local/bin/xmake
    globaldir: /root/.xmake
    tmpdir: /tmp/.xmake0/241028
    workingdir: /root/zcore
    packagedir: /root/.xmake/packages
    packagedir(cache): /root/.xmake/cache/packages/2410

The information of project:
    plat: linux
    arch: x86_64
    mode: release
    buildir: build
    configdir: /root/zcore/.xmake/linux/x86_64
    projectdir: /root/zcore
    projectfile: /root/zcore/xmake.lua

# 查看 target 信息 (例如 ldflags 来源)
$ xmake show -t ${target}
The information of target(main):
    at: /zelos/xmake/example/rules/protobuf.cpp/xmake.lua
    kind: binary
    targetfile: build/linux/x86_64/release/main
    deps:
      -> proto -> ./xmake.lua:22
    rules:
      -> mode.debug -> ./xmake.lua:1
      -> mode.release -> ./xmake.lua:1
      -> plugin.compile_commands.autoupdate -> ./xmake.lua:2
    links:
      -> protoc -> dep(proto) -> packages
      -> protobuf -> dep(proto) -> packages
    syslinks:
      -> pthread -> dep(proto) -> packages
    linkdirs:
      -> /root/.xmake/packages/p/protobuf-cpp/3.19.4/e71494ae9d544ed7a5ad809df77e8cf2/lib -> dep(proto) -> packages
    includedirs:
      -> build/.gens/proto/linux/x86_64/release/rules/protobuf/proto -> dep(proto)
    sysincludedirs:
      -> /root/.xmake/packages/p/protobuf-cpp/3.19.4/e71494ae9d544ed7a5ad809df77e8cf2/include -> dep(proto) -> packages
    files:
      -> main.cc -> ./xmake.lua:21
    compiler (cxx): /usr/bin/clang++
      -> -Qunused-arguments -m64
    linker (ld): /usr/bin/clang++
      -> -m64
    compflags (cxx):
      -> -Qunused-arguments -m64 -std=c++17 -Ibuild/.gens/proto/linux/x86_64/release/rules/protobuf/proto -isystem /root/.xmake/packages/p/protobuf-cpp/3.19.4/e71494ae9d544ed7a5ad809df77e8cf2/include
    linkflags (ld):
      -> -m64 -L/root/.xmake/packages/p/protobuf-cpp/3.19.4/e71494ae9d544ed7a5ad809df77e8cf2/lib -lprotobuf -lprotoc -lpthread
```

## Environments

```bash
$ xmake show -l envs
XMAKE_PROGRAM_DIR       Set the program scripts directory of xmake.
                        /usr/local/share/xmake
XMAKE_RCFILES           Set the runtime configuration files.
                        
XMAKE_RAMDIR            Set the ramdisk directory.
                        <empty>
XMAKE_PKG_INSTALLDIR    Set the install directory of packages.
                        ~/.xmake/packages
XMAKE_PKG_CACHEDIR      Set the cache directory of packages.
                        ~/.xmake/cache
XMAKE_ROOT              Allow xmake to run under root.
                        y
XMAKE_PROFILE           Start profiler, e.g. perf:call, perf:tag, trace, stuck.
                        <empty>
XMAKE_GLOBALDIR         Set the global config directory of xmake.
                        /root/.xmake
XMAKE_LOGFILE           Set the log output file path.
                        <empty>
XMAKE_CONFIGDIR         Set the local config directory of project.
                        /root/zelos/xmake/.xmake/linux/x86_64
XMAKE_TMPDIR            Set the temporary directory.
                        /tmp/.xmake0/241120
XMAKE_COLORTERM         Set the color terminal environment.
                        truecolor
```
