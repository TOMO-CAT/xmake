add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "." })

set_languages("c++17")

add_requires("protobuf-cpp 3.19.4")

target("proto", function()
    set_kind("static")
    -- proto_public = true 用于导出 proto 的头文件搜索目录, 开放给其他父 target 继承使用
    -- 在这个例子中是添加了 -Ibuild/.gens/proto/linux/x86_64/release/rules/protobuf/proto 编译参数
    --
    -- https://github.com/xmake-io/xmake/issues/828
    -- https://github.com/xmake-io/xmake/issues/1844
    -- proto_rootdir = "." 用于以 proto 的目录结构来生成对应的头文件目录结构
    add_files("proto/*.proto", { proto_public = true, proto_rootdir = "."})
    add_rules("protobuf.cpp")
    add_packages("protobuf-cpp", { public = true })
    -- 保证 target main 在 target proto link 完后才开始编译, 避免出现找不到 proto 头文件的 bug
    set_policy('build.fence', true)

    on_load(function(target)
        -- 支持导出 *.pb.h 头文件, 这样 xmake install 时可以导出到 include 目录
        -- FIXME: add_headerfiles 仅对 static target 生效, 对 object target 无效
        target:add("headerfiles", path.join(target:autogendir(), "rules",
            "protobuf", "(**.h)"))

        -- 支持按照 proto 文件所在的目录树 include *.pb.h 文件
        -- target:add("includedirs", path.join(target:autogendir(), "rules",
        --     "protobuf"), { public = true })
        -- 添加 `proto_rootdir = "proto"` 来适配这个问题
    end)
end)

target("main", function()
    set_kind("binary")
    add_files("main.cc")
    add_deps("proto")
end)

--[[
xmake -bvrD
xmake run main
]]
