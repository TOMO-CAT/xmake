-- These targets should compiled and linked concurrently.
add_rules("mode.release", "mode.debug", "mode.releasedbg")

-- 关闭 ccache 和 xcache (xmake ccache)
set_policy("build.ccache", false)
set_policy("build.xcache", false)

target("first", function()
    set_kind("binary")
    add_files("1.cpp")
    set_languages("clatest", "cxx20")
end)

target("second", function()
    set_kind("binary")
    add_files("2.cpp")
    set_languages("clatest", "cxx20")
end)
