# Compile

## 判断依赖信息

在 `xmake/module/private/action/build/object.lua` 文件中：

```lua
function _do_build_file(target, sourcefile, opt)
    ...

    -- rebuilt 表示 xmake b -r, 此时需要忽略 dependinfo 重新编译
    local dependinfo = target:is_rebuilt() and {} or (depend.load(dependfile, {target = target}) or {})

    ...
end
```

拿到的 dependinfo 信息例子如下：

```txt
{
    files = {
        "pnc/common/path_boundary/ra_path_boundary.cc"
    },
    depfiles_gcc = "build/.objs/pnc.common.path_boundary/linux/x86_64/release/pnc/common/path_boundary/ra_path_boundary.cc.o:   pnc/common/path_boundary/ra_path_boundary.cc   pnc/common/path_boundary/ra_path_boundary.h   pnc/common/range/l_range.hpp\
",
    values = {
        "/usr/bin/clang++",
        {
            "-Qunused-arguments",
            "-m64",
            "-O3",
            "-std=c++17",
            "-I.",
            "-isystem",
            "build/.pkg/zmath/include",
            "-isystem",
            "build/.pkg/zcore/include",
            "-isystem",
            "build/.pkg/zproto/include",
            "-isystem",
            "build/.pkg/protobuf/include",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-fPIC"
        }
    }
}
```

判断依赖信息是否需要重新编译的入口在 `xmake/modules/core/project/depend.lua` 中：

```lua
function is_changed(dependinfo, opt)
end
```

* 检查 `dependinfo.files` 里的每个文件，和 `dependfile` 的时间进行比较，如果有文件时间更新，则返回 true
* 检查 `dependinfo.values` 和当前编译的 `opt.values` 长度和内容是否完全一致（包含顺序）
* 检查 `dependinfo.files` 和当前的 `opt.files` 长度和内容是否完全一致（包含顺序）
