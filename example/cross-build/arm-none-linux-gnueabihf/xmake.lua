add_rules("mode.debug", "mode.release")

set_languages("c++17")

-- 编译出 host 版本的 protoc, 用于生成 *.pb.h 和 *.pb.cc
-- * 方案 1: 源码编译, 指定 host = true 就只会编译 host 版本的 protoc
-- add_requires("protobuf-cpp~host 3.19.4", {host = true, alias = "protoc"})
-- * 方案 2: 直接使用 protoc 预编译好的二进制, 节省编译速度
add_requires("protoc 3.19.4")

-- 编译出 cross 版本的 protobuf 库, 用于构造交叉编译成品库
-- `protoc = false` 会跳过编译 protoc, 因为交叉编译时本身就只需要 host protoc, 可以优化编译速度
add_requires("protobuf-cpp 3.19.4", {configs = {protoc = false}})

target("pb", function()
    set_kind("object")
    add_files("pb/*.proto", {proto_public = true})
    add_rules("protobuf.cpp")
    add_packages("protobuf-cpp", {public = true})

    -- 只是为了获取 host protoc, 不引入 linkflags 和 linkdirs
    -- 尤其是 protoc 是 host 版本, 引入 linkdirs 会找到错误 arch 的 lib 导致编译不过
    add_packages("protoc", {links = {}, linkdirs = {}})
    set_policy("build.fence", true)
end)

target("arm-none-linux-gnueabihf-cross", function()
    set_kind("binary")
    add_files("src/*.cc")
    add_deps("pb")
end)
