add_rules("mode.debug", "mode.release")

-- set cross-compiling platform
set_plat("cross")
set_arch("arm")

-- custom toolchain
toolchain("my_muslcc", function()
    set_homepage("https://musl.cc/")
    set_description("The musl-based cross-compilation toolchains")
    set_kind("cross")
    on_load(function(toolchain)
        toolchain:load_cross_toolchain()
        if toolchain:is_arch("arm") then
            toolchain:add("cxflags", "-march=armv7-a", "-msoft-float",
                          {force = true})
            toolchain:add("ldflags", "-march=armv7-a", "-msoft-float",
                          {force = true})
        end
        toolchain:add("syslinks", "gcc", "c")
    end)
end)

-- add library packages
-- for testing zlib/xmake, libplist/autoconf, libogg/cmake
add_requires("zlib", {system = false})

-- add toolchains package
add_requires("muslcc")

-- set global toolchains for target and packages
set_toolchains("my_muslcc@muslcc")

-- use the builtin toolchain("muslcc") instead of "my_muslcc"
-- set_toolchains("@muslcc")

target("test", function()
    set_kind("binary")
    add_files("src/*.c")
    add_packages("zlib")
end)
