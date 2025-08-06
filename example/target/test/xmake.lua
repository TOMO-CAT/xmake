add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

add_requires("gtest 1.13.0", {configs = {main = true}})

-- 老写法
target("old_foo_test", function()
    set_kind("binary")
    set_default(false)
    add_tests("default")
    set_rundir("$(projectdir)")
    add_files("src/foo_test.cc")
    add_packages("gtest")
end)

-- 新写法
-- * test 均是 binary target
-- * 默认 set_default(false), 避免默认编译和安装
-- * 默认 add_tests("default")
-- * 没调用 add_testfiles() 时默认设置 rundir 为 "$(projectdir)"
target("foo_test", function()
    set_kind("test")
    add_files("src/foo_test.cc")
    add_packages("gtest")

    -- 确保在没有调用 add_files() 时, rundir 是 os.projectdir()
    before_run(function(target)
        if target:rundir() ~= os.projectdir() then
            raise("invalid rundir")
        end
    end)
end)

-- 添加测试文件
-- * 调用 testfiles 后 rundir 转移到沙盒目录, 除了显式添加的 testfiles 访问不到其他文件
-- * path 会覆盖传入的路径
-- * 默认是 softlink 减少拷贝成本, 如果要修改文件可以设置 softlink 为 false 强制拷贝
target("foo_test_with_testfiles", function()
    set_kind("test")
    add_files("src/foo_test_with_testfiles.cc")
    add_packages("gtest")

    -- 默认场景, 软链 + 相同路径
    add_testfiles("foo.txt", {src_path = "foo.txt", softlink = true})
    add_testfiles("bar")

    -- 拷贝到新的目标路径
    add_testfiles("folder_1/folder_2/new_path.txt", {src_path = "test_data/new_path.txt"})
    add_testfiles("new_path.txt", {src_path = "test_data/new_path.txt"})

    -- 不使用软链, 直接拷贝
    add_testfiles("copy_foo.txt", {src_path = "foo.txt", softlink = false})
end)
