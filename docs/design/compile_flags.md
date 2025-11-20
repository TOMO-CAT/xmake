# 编译参数

## 入口

```bash
-- xmake/core/tool/compiler.lua
function compiler:compflags(opt)
    ...
end
```

## package 参数

```bash
# 入口
local compflags = compinst:compflags({target = target, sourcefile = sourcefile, configs = opt.configs})

# 构造编译参数
function compiler:compflags(opt)
    ...
    # 基于 target 构造编译参数, 这里包含 target 依赖的 package
    self:_add_flags_from_target(flags, target)
    ...
end

# 从 target:pkgs 构造编译参数
function builder:_add_flags_from_target(flags, target)
    ...
    self:_add_flags_from_targetpkgs(targetflags, target)
    ...
end
```

## 实际编译

```bash
# 编译入口
-- xmake/modules/core/tools/gcc.lua
function compile(self, sourcefile, objectfile, dependinfo, flags, opt)

end

# 实际编译
-- xmake/modules/core/tools/gcc.lua
function _compile(self, sourcefile, objectfile, compflags, opt)
    ...
    local program, argv = compargv(self, sourcefile, objectfile, compflags, opt)
    ...
end
```
