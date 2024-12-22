includes("toolchain.lua")

add_rules("mode.debug", "mode.release")

set_languages("c++17")

-- 编译出 host 版本的 protoc, 用于生成 *.pb.h 和 *.pb.cc
-- 这种写法由于指定了 toolchain, 所以是无法构造出来 host 版本的 protoc, 只能将 protoc 拷贝到 docker 里
-- add_requires("protobuf-cpp~host 3.19.4", {host = true, alias = "protoc"})

target("g++-9-aarch64-linux-gnu-cross", function()
    set_kind("binary")
    add_files("src/*.cc")
end)
