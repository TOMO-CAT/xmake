-- project
set_project("xmake")

-- version
set_version("3.0.4", {build = "%Y%m%d"})

-- set xmake min version
set_xmakever("2.8.5")

-- includedirs
add_includedirs("src", {public = true})

-- set all warnings as errors
set_warnings("all", "error")

-- set language: c99, c++11
set_languages("c99", "cxx11")

-- add release and debug modes
add_rules("mode.release", "mode.debug")
if is_mode("release") then
    set_optimize("smallest")
end

-- generate compile_commands.json
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

-- disable some compiler errors
add_cxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=nullability-completeness", "-Wno-error=parentheses-equality")

-- add definitions
add_defines("_GNU_SOURCE=1", "_FILE_OFFSET_BITS=64", "_LARGEFILE_SOURCE")

-- add vectorexts
--[[
if is_arch("x86", "x64", "i386", "x86_64") then
    add_vectorexts("sse", "sse2", "sse3", "avx", "avx2")
elseif is_arch("arm.*") then
    add_vectorexts("neon")
end]]

-- for mode coverage
if is_mode("coverage") then
    add_ldflags("-coverage", "-fprofile-arcs", "-ftest-coverage")
end

-- the runtime option
option("runtime")
    set_default("lua")
    set_description("Use luajit or lua runtime")
    set_values("luajit", "lua")
option_end()

-- the lua-cjson option
option("lua_cjson")
    set_default(true)
    set_description("Use lua-cjson as json parser")
option_end()

-- the readline option
option("readline")
    set_description("Enable or disable readline library")
    add_links("readline")
    add_cincludes("stdio.h", "readline/readline.h")
    add_cfuncs("readline")
    add_defines("XM_CONFIG_API_HAVE_READLINE")
option_end()

-- the curses option
option("curses")
    set_description("Enable or disable curses library")
    before_check(function (option)
        option:add("cincludes", "curses.h")
        option:add("links", "curses")
    end)
    add_defines("XM_CONFIG_API_HAVE_CURSES")
option_end()

-- only build xmake libraries for development?
option("onlylib")
    set_default(false)
    set_description("Only build xmake libraries for development")
option_end()

-- add projects
includes("src/sv", "src/lz4", "src/tbox", "src/xmake", "src/demo")
if has_config("lua_cjson") then
    includes("src/lua-cjson")
end
if is_config("runtime", "luajit") then
    includes("src/luajit")
else
    includes("src/lua")
end

-- add xpack
includes("@builtin/xpack")
if xpack then
    includes("xpack.lua")
end
