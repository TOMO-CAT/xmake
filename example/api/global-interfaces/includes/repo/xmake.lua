-- 引用远程 repo 自定义脚本
add_repositories("cat-repo https://github.com/zxmake/cat-zxmake-repo.git")
includes("@cat-repo/rules/cat.release_mode.lua")
includes("@cat-repo/rules/cat.check_test_target.lua")

-- 引用本地 repo 自定义脚本
add_repositories("my-repo my-repo", {rootdir = os.scriptdir()})
includes("@my-repo/rules/my-repo.check_test_target.lua")

target("test", function()
    set_kind("binary")
    add_tests("default")
    add_files("main.cc")
    add_rules("cat.release_mode")
    add_rules("my-repo.check_test_target")
end)
