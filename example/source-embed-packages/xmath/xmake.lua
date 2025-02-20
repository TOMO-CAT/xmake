add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_cxxflags("-Wall", "-Wextra", "-Werror")
add_includedirs(".")

-- 添加本地源码库依赖
add_repositories("local-xmake-repo ../local-xmake-repo")
add_requires("xcore", {system = false})

target("xmath", function()
    set_kind("static")
    add_files("math/*.cc")
    add_headerfiles("(math/*.h)")
    add_packages("xcore")
end)
