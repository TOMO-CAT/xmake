includes("@builtin/check")

-- 检测完会给代码添加宏定义: -DLONG_SIZE=8 -DSTRING_SIZE=32
target("test", function()
    set_kind("binary")
    add_files("main.cc")
    check_sizeof("LONG_SIZE", "long")
    check_sizeof("STRING_SIZE", "std::string", {includes = "string"})
end)

-- output
--
-- checking for LONG_SIZE ... 8
-- checking for STRING_SIZE ... 32
