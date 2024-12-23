add_rules("mode.debug", "mode.release")

add_requires("muslcc")

-- 等价于 xmake f -p cross --arch=arm --toolchain=muslcc
set_plat("cross")
set_arch("arm")
set_toolchains("@muslcc")

add_requires("zlib", "libogg", {system = false})

target("test", function()
    set_kind("binary")
    add_files("src/*.c")
    add_packages("zlib", "libogg")
end)
