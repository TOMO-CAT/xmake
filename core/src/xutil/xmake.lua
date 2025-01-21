-- add module directories
add_moduledirs("xmake")

-- generate compile_commands.json
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

-- project
set_project("xutil")

-- set xmake minimum version
set_xmakever("3.0.2")

-- set project version
set_version("1.0.0", {build = "%Y%m%d", soname = true})

-- set warning all as error
set_warnings("all", "error")

-- set language: c99
stdc = "c99"
set_languages(stdc)

-- add defines to config.h
set_configvar("_GNU_SOURCE", 1)
set_configvar("_REENTRANT", 1)

-- disable some compiler errors
add_cxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing",
            "-Wno-error=expansion-to-defined", "-Wno-error=empty-body")
add_mxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing",
            "-Wno-error=expansion-to-defined", "-Wno-error=empty-body")

-- unit test
if has_config("ut") then
    includes("unit_test/xmake.lua")
end

-- set wasm toolchain
if is_plat("wasm") then
    add_requires("emscripten")
    set_toolchains("emcc@emscripten")
end

-- add build modes
add_rules("mode.release", "mode.debug", "mode.profile", "mode.coverage",
          "mode.valgrind", "mode.asan", "mode.tsan", "mode.ubsan")
if is_mode("debug") then
    add_defines("__xu_debug__")
end
if is_mode("valgrind") then
    add_defines("__xu_valgrind__")
end
if is_mode("asan") then
    add_defines("__xu_sanitize_address__")
end
if is_mode("tsan") then
    add_defines("__xu_sanitize_thread__")
end

-- small or micro?
if has_config("small", "micro") then
    add_defines("__xu_small__")
    set_configvar("XU_CONFIG_SMALL", 1)
    if is_mode("release", "profile") then
        set_optimize("smallest")
    end
    add_cxflags("-fno-stack-protector")
end

if is_plat("android") then
    add_syslinks("m", "c")
elseif is_plat("haiku") then
    add_syslinks("pthread", "network", "m", "c")
else
    add_syslinks("pthread", "dl", "m", "c")
end

-- enable backtrace symbols for linux
if is_plat("linux") and is_mode("debug") then
    add_ldflags("-rdynamic")
end

target("xutil", function()
    -- make as a static/shared library
    set_kind("object")

    -- install importfiles for pkg-config/cmake
    add_rules("utils.install.cmake_importfiles")
    add_rules("utils.install.pkgconfig_importfiles")

    -- add defines
    add_defines("__xu_prefix__=\"xutil\"")

    -- set the auto-generated config.h
    set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)")
    add_configfiles("xutil.config.h.in")

    -- add include directories
    add_includedirs("..", {public = true})
    -- xutil.config.h
    if os.exists(path.join("inc", os.host())) then
        add_includedirs(path.join("inc", os.host()), {public = true})
    else
        add_includedirs("$(buildir)/$(plat)/$(arch)/$(mode)", {public = true})
    end

    -- add frameworks
    if is_plat("macosx") then
        add_frameworks("CoreFoundation", "CoreServices", {public = true})
    end

    -- option@force-utf8: Force regard all xu_char* as utf-8.
    set_configvar("XU_CONFIG_FORCE_UTF8", 1)
    -- option@info: Enable or disable to get some info, .e.g version ..
    if not has_config("small") and not has_config("micro") then
        set_configvar("XU_CONFIG_INFO_HAVE_VERSION", 1)
        set_configvar("XU_CONFIG_INFO_TRACE_MORE", 1)
    end
    -- option@float: Enable or disable the float type
    if not has_config("micro") then
        set_configvar("XU_CONFIG_TYPE_HAVE_FLOAT", 1)
    end
    -- option@hash
    set_configvar("XU_CONFIG_MODULE_HAVE_HASH", 1)
    -- option@charset
    set_configvar("XU_CONFIG_MODULE_HAVE_CHARSET", 1)

    -- add the common source files
    add_files("*.c")
    add_files("algorithm/**.c")
    add_files("container/**.c")
    add_files("libc/**.c")
    add_files("math/**.c")
    add_files("memory/**.c")
    add_files("network/**.c")
    add_files("platform/*.c", "platform/impl/*.c")
    add_files("stream/**.c")
    add_files("string/**.c")
    add_files("utils/*.c")
    add_files("libm/**.c")
    add_files("charset/**.c")

    -- add the source files for the hash module
    add_files("hash/*.c")

    -- check interfaces
    on_config("check_interfaces")
end)
