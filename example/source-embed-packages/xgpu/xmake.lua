add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_cxxflags("-Wall", "-Wextra", "-Werror")
add_includedirs(".")

-- 添加本地源码库依赖
add_repositories("local-xmake-repo ../local-xmake-repo")
add_requires("xmath", {system = false})

target("xgpu", function()
    set_kind("binary")
    add_files("main.cc")
    add_packages("xmath")
end)
