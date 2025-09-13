add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_includedirs(os.projectdir())

set_policy("build.across_targets_in_parallel", true)
set_policy("build.ccache", false)
set_policy("build.xcache", false)

target("1", function()
    set_kind("object")
    add_files("src/1.cc")
end)

target("2", function()
    set_kind("object")
    add_files("src/2.cc")
    add_deps("1")
end)

target("3", function()
    set_kind("object")
    add_files("src/3.cc")
    add_deps("2")
end)

-- main 链接时必须等待间接依赖 1 和 2 build 完成
target("main", function()
    set_kind("static")
    add_files("src/main.cc")
    add_deps("3")
end)

target("4", function()
    set_kind("object")
    add_files("src/4.cc")
    add_deps("3")
end)

target("5", function()
    set_kind("object")
    add_files("src/5.cc")
    add_deps("4")
end)
