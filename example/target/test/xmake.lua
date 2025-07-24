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
end)
