add_rules("mode.debug", "mode.release")

add_repositories("cat-repo https://github.com/zxmake/cat-zxmake-repo.git")

set_languages("c++17")
add_cxxflags("-Wall", "-Wextra", "-Werror")
add_includedirs(".")

-- 添加本地源码库依赖
add_repositories("local-xmake-repo ../local-xmake-repo")
add_requires("xperc", {system = false})

target("xsim", function()
    set_kind("binary")
    add_files("main.cc")
    add_packages("xperc")
end)
