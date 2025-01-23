add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_cxxflags("-Wall", "-Wextra", "-Werror")
add_includedirs(".")

-- 添加本地源码库依赖
add_repositories("local-xmake-repo ../local-xmake-repo")
add_requires("xproto", {system = false})

target("xcore", function()
    set_kind("static")
    add_files("util/time.cc")
    add_headerfiles("(util/time.h)")
    add_packages("xproto")
end)
