add_rules("mode.debug", "mode.release")

add_repositories("cat-repo https://github.com/zxmake/cat-zxmake-repo.git")
-- 新版本的 builtin rules
add_rules("cat.release_mode")

set_languages("c++17")
add_cxxflags("-Wall", "-Wextra", "-Werror")
add_includedirs(".")

-- 禁用 ccache, 否则编译会比较快
set_policy("build.ccache", false)
set_policy("build.xcache", false)

target("xperc-common", function()
    set_kind("object")
    add_files("perc/*.cc")
end)

target("xperc", function()
    set_kind("static")
    add_files("perc/perc.cc")
    add_headerfiles("(perc/*.h)")
end)
