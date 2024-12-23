add_rules("mode.debug", "mode.release")

set_languages("c++17")

-- 编译出 host 版本的 protoc, 用于生成 *.pb.h 和 *.pb.cc
add_requires("protobuf-cpp~host 3.19.4", {host = true, alias = "protoc"})
-- 编译出 cross 版本的 protobuf 库, 用于构造交叉编译成品库
add_requires("protobuf-cpp 3.19.4")

target("pb", function()
    set_kind("object")
    add_files("pb/*.proto", { proto_public = true})
    add_rules("protobuf.cpp")
    add_packages("protobuf-cpp", {public = true})
    add_packages("protoc", {links = {}}) -- 只是为了获取 host protoc, 不引入 linkflags
    set_policy('build.fence', true)
end)

target("aarch64-none-linux-gnu-cross", function()
    set_kind("binary")
    add_files("src/*.cc")
    add_deps("pb")
end)
