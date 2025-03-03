-- 使用此插件可以生成 a.cc 和 b.cc 的 compile-commands
-- add_rules("plugin.compile_commands.autoupdate", { outputdir = "." })

target("test", function()
    add_files("a.cc")

    -- 引入额外的包含 main 入口函数的单测文件, 所以我们需要移除已有的包含 main 入口的 a.cc 文件
    add_tests("test", {files = "b.cc", remove_files = "a.cc"})
end)
