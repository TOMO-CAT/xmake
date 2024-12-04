add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_includedirs(os.projectdir())

set_policy("build.across_targets_in_parallel", true)
set_policy("build.ccache", false)
set_policy("build.xcache", false)

-- add_cxxflags("-ftemplate-depth=1000")
-- add_cxxflags("-Wno-stack-exhausted")

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
