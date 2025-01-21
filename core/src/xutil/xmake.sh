#!/bin/bash

# project
set_project "xutil"
set_version "1.0.0" "%Y%m%d" "1"

# set warning all as error
set_warnings "all" "error"

# set language: c99
set_languages "c99"

# add defines to config.h
set_configvar "_GNU_SOURCE" 1
set_configvar "_REENTRANT" 1

# add build modes
if is_mode "debug"; then
    add_defines "__xu_debug__"
    set_symbols "debug"
    set_optimizes "none"
else
    set_strip "all"
    if ! is_kind "shared"; then
        set_symbols "hidden"
    fi
    set_optimizes "smallest"
fi

# small or micro?
if has_config "small"; then
    add_defines "__xu_small__"
    set_configvar "XU_CONFIG_SMALL" 1
    add_cxflags "-fno-stack-protector"
fi

if is_plat "haiku"; then
    add_syslinks "pthread" "network" "m" "c"
else
    add_syslinks "pthread" "dl" "m" "c"
fi

# enable backtrace symbols for linux
if is_plat "linux" && is_mode "debug"; then
    add_ldflags "-rdynamic"
fi

basic_options() {
    # option@force-utf8: Force regard all xu_char* as utf-8.
    set_configvar "XU_CONFIG_FORCE_UTF8" 1

    # option@info: Enable or disable to get some info, .e.g version ..
    if ! has_config "small" && ! has_config "micro"; then
        set_configvar "XU_CONFIG_INFO_HAVE_VERSION" 1
        set_configvar "XU_CONFIG_INFO_TRACE_MORE" 1
    fi

    if has_config "micro"; then
        # option@micro
        set_configvar "XU_CONFIG_MICRO_ENABLE" 1
    else
        # option@float: Enable or disable the float type
        set_configvar "XU_CONFIG_TYPE_HAVE_FLOAT" 1
    fi

    # option@hash
    set_configvar "XU_CONFIG_MODULE_HAVE_HASH" 1
    # option@charset
    set_configvar "XU_CONFIG_MODULE_HAVE_CHARSET" 1
}
basic_options

if has_config "ut"; then
    includes "unit_test"
fi

target "xutil"
    set_kind "${kind}"

    # add defines
    add_defines "__xu_prefix__=\"xutil\""

    # use generated xutil.config.h for each platform
    add_includedirs "inc/${plat}" "{public}"

    # add include directories
    add_includedirs ".." "{public}"

    # add frameworks
    if is_plat "macosx"; then
        add_frameworks "CoreFoundation" "CoreServices" "{public}"
    fi

    add_files "*.c"
    add_files "algorithm/**.c"
    add_files "container/**.c"
    add_files "libc/**.c"
    add_files "math/**.c"
    add_files "memory/**.c"
    add_files "network/**.c"
    add_files "platform/*.c"
    add_files "platform/impl/*.c"
    add_files "stream/**.c"
    add_files "string/**.c"
    add_files "utils/*.c"
    add_files "libm/**.c"
    add_files "charset/**.c"
    add_files "hash/*.c"
