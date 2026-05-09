includes("toolchain.lua")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
add_rules("mode.debug", "mode.release")

add_cxxflags("-Wall", "-Wextra", "-Werror")
add_cxxflags("-Wno-unused-parameter")

set_languages("c++17")

if os.getenv("GITHUB_ACTIONS") then
    -- 在 github action 时跑一下比较简单的版本, 避免流水线耗时过长
    target("clang-sysroot-target", function()
        set_kind("binary")
        add_files("src/main.cc")
    end)
else
    -- 编译出 host 版本的 protoc, 用于生成 *.pb.h 和 *.pb.cc
    -- * 方案 1: 源码编译, 指定 host = true 就只会编译 host 版本的 protoc
    -- add_requires("protobuf-cpp~host 3.19.4", {host = true, alias = "protoc"})
    -- * 方案 2: 直接使用 protoc 预编译好的二进制, 节省编译速度
    add_requires("protoc 3.19.4")

    -- 编译出 cross 版本的 protobuf 库, 用于构造交叉编译成品库
    -- `protoc = false` 会跳过编译 protoc, 因为交叉编译时本身就只需要 host protoc, 可以优化编译速度
    add_requires("protobuf-cpp 3.19.4", {configs = {protoc = false}})

    target("proto", function()
        set_kind("object")
        add_files("pb/*.proto", {proto_public = true})
        add_rules("protobuf.cpp")
        add_packages("protobuf-cpp", {public = true})
        -- 使用 package 的 protoc 而不是系统目录下的 protoc
        add_packages("protoc", {links = {}, linkdirs = {}})
        set_policy('build.fence', true)
    end)

    target("clang-sysroot-target", function()
        set_kind("binary")
        add_files("src/proto_main.cc")
        add_deps("proto")
    end)
end
