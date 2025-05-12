-- 定义 dev config, 可以通过 xmake config --dev=true 启用
option("dev", function()
    set_default(false)
end)

-- 1) 通过 option 来控制编译选项
if get_config("dev") then
    -- dev 为 true 时，启用调试信息
    add_cxxflags("-g")
    add_cxxflags("-O0")
else
    -- dev 为 false 时，启用优化 (默认行为)
    add_cxxflags("-O3")
end

-- 2) 通过 option 来控制 target 是否启用
if get_config("dev") then
    target("foo", function()
        set_kind("phony")
        on_load(function(target)
            print("=== foo ===")
        end)
    end)
else
    target("bar", function()
        set_kind("phony")
        on_load(function(target)
            print("=== bar ===")
        end)
    end)
end

-- 3) 对于 set_config 这种最顶层 api, 无法支持在 option 判断中动态切换, 可以封装在 custom toolchain, 通过 set_toolchain 做切换

toolchain("aarch64", function()
    -- 替换 set_config("sdk", "/usr")
    set_sdkdir("/usr")

    -- 替换 set_config("cross", "aarch64-linux-gnu-")
    set_cross("aarch64-linux-gnu-")
end)

if get_config("dev") then
    -- 但是 set_toolchains, set_plat 和 set_arch 这种 api 是可以支持在 option 判断中动态切换的
    set_plat("cross")
    set_arch("arm64-v8a")

    -- FIXME: 尽管设置了 toolchain.set_cross, 但是并没有切换到 /usr/bin/aarch64-linux-gnu-, 我们选择用 xmake config 命令来设置, 后续再调试
    --        xmake config -p cross -a arm64-v8a --sdk=/usr --cross=aarch64-linux-gnu-
    -- set_toolchains("aarch64")
end

target("test", function()
    set_kind("binary")
    add_files("main.cc")
end)
