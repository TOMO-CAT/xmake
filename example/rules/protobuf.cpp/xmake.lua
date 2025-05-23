add_rules("mode.debug", "mode.release")
-- add_rules("plugin.compile_commands.autoupdate", { outputdir = "." })

set_languages("c++17")

add_requires("protobuf-cpp 3.19.4")

target("foo.proto", function()
    set_kind("object")

    -- 优化 protobuf.cpp 规则报错信息, 展示成深红色
    -- @see https://github.com/TOMO-CAT/xmake/issues/24
    -- 这里增加一个不存在的 -I protoc 参数来模拟报错
    add_files("foo/proto/*.proto", { proto_public = true, proto_rootdir = "foo", extra_flags = "-Ino-exist-folder" })
    add_rules("protobuf.cpp")
    add_packages("protobuf-cpp", {public = true})
    set_policy('build.fence', true)
end)

target("test.proto", function()
    set_kind("object")

    -- proto_public = true 用于导出 proto 的头文件搜索目录, 开放给其他父 target 继承使用
    -- 在这个例子中是添加了 -Ibuild/.gens/proto/linux/x86_64/release/rules/protobuf/proto 编译参数
    --
    -- @see https://github.com/xmake-io/xmake/issues/828
    -- https://github.com/xmake-io/xmake/issues/1844
    -- https://github.com/TOMO-CAT/xmake/issues/22
    -- proto_rootdir = "." 用于以 proto 的目录结构来生成对应的头文件目录结构
    --
    -- extra_flags = { "-Ifoo" } 用于添加 protoc 额外的编译参数
    -- @see https://github.com/TOMO-CAT/xmake/issues/23
    add_files("proto/*.proto", { proto_public = true, proto_rootdir = ".", extra_flags = { "-Ifoo" } })

    add_rules("protobuf.cpp")
    add_packages("protobuf-cpp", { public = true })

    -- 保证 target main 在 target proto link 完后才开始编译, 避免出现找不到 proto 头文件的 bug
    set_policy('build.fence', true)
    add_deps("foo.proto")

    on_load(function(target)
        -- 支持导出 *.pb.h 头文件, 这样 xmake install 时可以导出到 include 目录
        -- add_headerfiles 仅对 static target 生效, 对 object target 无效
        target:add("headerfiles", path.join(target:autogendir(), "rules",
            "protobuf", "(**.h)"))
    end)
end)

target("main", function()
    set_kind("binary")
    add_files("main.cc")
    add_deps("test.proto")
end)
