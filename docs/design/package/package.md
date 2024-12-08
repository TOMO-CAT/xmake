# package

## 如何检测循环依赖?

xmake 为每个 package 都准备了 `requirepath` 字段。

> A -> B -> C

判断循环依赖的逻辑在 `xmake/modules/private/action/require/impl/package.lua` 中：

```lua
-- load required packages
function _load_package(packagename, requireinfo, opt)

    -- check circular dependency
    opt = opt or {}
    if opt.requirepath then
        local splitinfo = opt.requirepath:split(".", {plain = true})
        if #splitinfo > 3 and
            splitinfo[1] == splitinfo[#splitinfo - 1] and
            splitinfo[2] == splitinfo[#splitinfo] then
            raise("circular dependency(%s) detected in package(%s)!", opt.requirepath, splitinfo[1])
        end
    end

    ...

end
```

假设有两个 package A 和 B 相互依赖，那么在上面的逻辑中 `opt.requirepath` 会先后变为：

* `A`
* `A.B`
* `A.B.A`
* `A.B.A.B`

此时就可以检测出来循环依赖了，但是感觉这种方式不是那么优雅，有空再改进一下。

## package 类型

### 1. thirdparty

* brew::pcre2/libpcre2-8
* conan::OpenSSL/1.0.2n@conan/stable


## 加载 package

### 1. 从 repo 里加载

入口在 `xmake/core/package/package.lua`：

```lua
-- load the package from the package directory or package description file
function package.load_from_repository(packagename, packagedir, opt)
    ...
end
```
