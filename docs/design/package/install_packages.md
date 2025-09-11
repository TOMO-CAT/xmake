# install packages

## 入口

我们会在代码中 `add_requires`，比如：

```bash
-- remote xmake repo 引入的 package
add_requires("gtest 1.13.0", {configs = {main = true}})
add_requires("protobuf 21.7")

-- local xmake repo
add_repositories("local-xmake-repo ../local-xmake-repo", {rootdir = os.projectdir()})

-- local xmake repo 引入的 package
add_requires("zelos")

-- 使用 set_sourcedir 的 local package
add_requires("zproto")
```

入口函数在 `xmake/modules/private/action/require/install.lua`：

```lua
function main(requires_raw)
    ...

    -- @requires: {"protobuf 21.7", "zproto", "gtest 1.13.0", "zelos"}
    -- @requires_extra: {"gtest 1.13.0": {configs: {main = true}}}
    local packages = install_packages(requires, {requires_extra = requires_extra})

    ...
end)
```

## load packages

基于给定的 requires 加载所有的 package。

### 1. load_requires

`load_packages` 的第一步是 `load_requires`，它会将所有用户 `add_requires` 后的结果加载成 package-name 和 require-info 的 require-item。

```lua
function load_requires(requires, requires_extra, opt)
    opt = opt or {}
    local requireitems = {}
    for _, require_str in ipairs(requires) do
        local packagename, requireinfo = _load_require(require_str, requires_extra, opt.parentinfo)
        table.insert(requireitems, {name = packagename, info = requireinfo})
    end
    return requireitems
end
```

### 2. 基于 project 里的 root requires 信息加载全部 package

`load_package` 主要逻辑：

* 解析出来 tag（例如 `zlib~debug` 的 tag 是 debug）存储到 `requireinfo.label` 中
* 计算出来 `requirekey`（格式是 `${requirestr}#${hash(require)}`），确保能区分出来不同 config 的 package，存储到 `requireinfo.requirekey` 中
* 先从当前 project 里定义的 package 加载，成功返回 `core.package.package` 的一个 instance
* 查找不到的话尝试从 repository 里定义的 package 加载
* 如果有 package 且有 `base` 属性的话尝试从 base 里加载 package 信息
* 初始化 requireinfo
  * 如果是 project 引入的 require package，设置 requireinfo.is_toplevel 为 true
  * 如果是 toplevel package，根据情况给 requireinfo.configs 中添加 toolchains / runtimes / lto / asan 等信息
* 将 `add_requireconfs` 的配置合并到 requireinfo 中
* 继承 parent-package 的 configs
* 将 requireinfo 设置到 package 的 `self._REQUIREINFO` 中
* 调用 `package:on_source` 方法
* 选择 package 合适的 version（会受到 locked—requireinfo 影响）
* 基于 package 的 requireinfo 和 version 构造出一个 packagekey
* 构造 displayname（pakcagename + packageid）并设置 `package:_DISPLAYNAME`
* 如果 package 的 cachedirs 冲突则关闭 parallelize
* 添加一些 builtin configurations（debug / shared / pic 等），并进行检查
* 选择 package runtime
* 计算出 package 的 buildhash（存储在 `~/.xmake/packages` 第二个路径中）
* 运行 `package:load` 方法
* 缓存到 `"require.impl.package"` memcache 里的 `"packages"` 缓存里

### 3. get package installdeps

获取每个 package 的 installdeps，然后按照 installdeps 给 package 排序，方便后续安装 package。

### 4. fetch packages

首先判断是否需要 fetch：

```lua
-- is_fetchonly: 表示只从系统获取，不会从远程下载安装
-- option.force
-- option.shallow
function _should_fetch_package(instance)
    if instance:is_fetchonly() or not option.get("force") or
        (option.get("shallow") and not instance:is_toplevel()) then
        return true
    end
end
```

然后调用 `package:fetch` 方法去 fetch package，写入到 `slef._FETCHINFO` 中。

* binary / toolchain package 调用 `self._fetch_tool`
* 其他 package 调用 `self._fetch_library`，对于 xmake repo 而言会调用 `package.manager.xmake.find_package`

这里只介绍 xmake-package-manager 的逻辑：

* 从 `package:installdir` 中查找 `"manifest.txt"`，如果查找不到则 fetch 失败
* 如果 `manifest.vars` 中有 includedirs 则直接写入 `fetchinfo.includedirs`，没有的话如果 installdir 中有 include 文件夹也可以写入到 `fetchinfo.includedirs`
* 同样我们可以自动扫描得到：
  * `fetchinfo.links`
  * `fetchinfo.linkdirs`
  * `fetchinfo.libfiles`
  * `fetchinfo.shared`  / `fetchinfo.static`：是否有动态库 / 静态库
  * `fetchinfo.syslinks`
* 对 fetchinfo 拿到的结果去重
* 更新 project references file（考虑在流水线中关闭这个功能？）
* 继续写入：
  * `fetchinfo.version`
  * `fetchinfo.license`
  * `fetchinfo.extra`

* 对于 external 设置为 true 的 package，那么将 `fetchinfo.includedirs` 抹去写到 `fetchinfo.sysincludedirs`

fetch 完 package 后会将结果写入到 detechcache 中（`.xmake/linux/x86_64/cache/detech` 文件），以 `add_requires("gtest v1.13.0")` 为例，写入的 的detechcache 内容为：

```txt
{
    find_package_linux_x86_64_fetch_package_xmake = {
        ["xmake::gtest_7de636c1cc2540f88b239e2073b4d4ee_release_v1.13.0_external"] = {
            license = "BSD-3",
            libfiles = {
                "/root/.xmake/packages/g/gtest/v1.13.0/7de636c1cc2540f88b239e2073b4d4ee/lib/libgmock.a",
                "/root/.xmake/packages/g/gtest/v1.13.0/7de636c1cc2540f88b239e2073b4d4ee/lib/libgtest.a",
                "/root/.xmake/packages/g/gtest/v1.13.0/7de636c1cc2540f88b239e2073b4d4ee/lib/libgtest_main.a"
            },
            links = {
                "gmock",
                "gtest",
                "gtest_main"
            },
            version = "v1.13.0",
            linkdirs = {
                "/root/.xmake/packages/g/gtest/v1.13.0/7de636c1cc2540f88b239e2073b4d4ee/lib"
            },
            sysincludedirs = {
                "/root/.xmake/packages/g/gtest/v1.13.0/7de636c1cc2540f88b239e2073b4d4ee/include"
            },
            syslinks = "pthread",
            static = true
        },
        ...
    }
}
```

### 5. register packages

将所有需要的 required-root-package （即 top-level-package）注册到 local-cache。

首先调用 `project.required_package(package-name)` 拿到 require-package 信息。然后注册 require-package 信息：

* package-base：installdir
* package-libs：includedirs / links / linkdirs / ...
* package-envs

然后继续添加所有依赖库的 package-libs 和 package-envs 信息。

以 protobuf 为例，它会将数据写入到 `.xmake/linux/x86_64/cache/package` 中：

```txt
{
   ...

    protobuf = {
        __requirestr = "protobuf 21.7",
        links = {
            "protobuf",
            "z"
        },
        envs = {
            PATH = {
                "/root/.xmake/packages/p/protobuf/21.7/ca596a052d5c44908e1dfd6f555b9a50/bin",
                "/root/.xmake/packages/c/cmake/3.29.2/c093867d5a85460b8d0b71eca73aaba7/bin"
            },
            LD_LIBRARY_PATH = {
                "/root/.xmake/packages/p/protobuf/21.7/ca596a052d5c44908e1dfd6f555b9a50/lib",
                "/root/.xmake/packages/c/cmake/3.29.2/c093867d5a85460b8d0b71eca73aaba7/lib",
                "/root/.xmake/packages/z/zlib/v1.3.1/994fafa590ed48ac9f71516cc846d155/lib"
            }
        },
        sysincludedirs = "/root/.xmake/packages/p/protobuf/21.7/ca596a052d5c44908e1dfd6f555b9a50/include",
        __enabled = true,
        libfiles = {
            "/root/.xmake/packages/p/protobuf/21.7/ca596a052d5c44908e1dfd6f555b9a50/lib/libprotobuf.a",
            "/root/.xmake/packages/p/protobuf/21.7/ca596a052d5c44908e1dfd6f555b9a50/lib/libprotobuf-lite.a",
            "/root/.xmake/packages/p/protobuf/21.7/ca596a052d5c44908e1dfd6f555b9a50/lib/libprotoc.a"
        },
        version = "21.7",
        linkdirs = "/root/.xmake/packages/p/protobuf/21.7/ca596a052d5c44908e1dfd6f555b9a50/lib",
        installdir = "/root/.xmake/packages/p/protobuf/21.7/ca596a052d5c44908e1dfd6f555b9a50",
        static = true
    },

   ...
}
```

同时我们将使用到的非 system 和 thirdparty 的 package 都写入到 reference-cache 中，它的存储位置在 `.xmake/linux/x86_64/cacahe/reference` 中。

### 6. install packages

首先判断是否应该 install package：

* `package:kind()` 为 template 的不安装
* 如果 package 开启了 `"package.install_always"` policy 那么需要安装
* 如果 `package:exists()` 为 true（即之前已经 fetch package 拿到了 fetchinfo），那么不需要安装
* `fetch_only` 的 package 不安装
* `add_requires("xxx", {system = true})`  只从系统库获取的 package 不安装
* 如果 `package:parents` 都已经安装完，那么不需要安装了

安装 package （入口在 `xmake/modules/private/action/require/impl/install_packages.lua`），调用 `runjobs` 并发安装所有的 packages，对于每一个 package 而言：

* 等待依赖的 package 安装完成
* 运行 download 脚本下载
* 运行 install 脚本
  * 先删除 `package:installdir` 和 `installdir.failed`
  * download package source（即 `package:add_resources` 引入的资源）
  * patch sources
  * 检查 `package:toolchains`
  * 运行 `action_install`：
  * 运行  `package:on_install` 脚本
  * 安装 rules
  * 将 package-info 保存到 manifest 文件中
  * fetch-package 且 force to flush the cache
  * patch pkg-config
  * test package
* 如果是 top-level package 的话 register package

安装完 package 后再次 register-packages，refresh 全部 root-packages 到 local-cache。

### 7. lock packages

对于开启 `package.requires_lock` 策略的 project，需要将 package 信息写入到 project 的 `xmake-requires.lock` 文件中。
