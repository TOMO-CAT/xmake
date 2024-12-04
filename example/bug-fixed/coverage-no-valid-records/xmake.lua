add_rules("mode.debug", "mode.release", "mode.coverage")

set_languages("c++17")

add_requires("gtest")

-- 不禁用 xmake build-cache 时可能会报错: no valid records found in tracefile coverage/coverage.info
-- 复现方法: 不删除 ~/.xmake/.build_cache 缓存目录, 只删除 build 目录重新编译
--
-- set_config("xcache", false)

target("time_checker_test", function()
    set_kind("binary")
    add_tests("default")
    add_files("src/time_checker_test.cc")
    add_packages("gtest")
end)
