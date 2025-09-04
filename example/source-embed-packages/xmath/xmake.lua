add_rules("mode.debug", "mode.release")

add_repositories("cat-repo https://github.com/zxmake/cat-zxmake-repo.git")
-- 新版本的 builtin rules
add_rules("cat.release_mode")

-- 老版本的 package rules
-- add_requires("cat", {system = false})
-- add_rules("@cat/release_mode")

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
