target("xmake", function()
    set_kind("static")

    -- add deps
    add_deps("sv", "lz4", "tbox", "xutil")
    if is_config("runtime", "luajit") then
        add_deps("luajit")
    else
        add_deps("lua")
    end
    if has_config("lua_cjson") then
        add_deps("lua-cjson")
    end

    -- add definitions
    add_defines("__tb_prefix__=\"xmake\"")
    if is_mode("debug") then
        add_defines("__xm_debug__", {public = true})
    end
    add_defines("__xm_prefix__=\"xmake\"")

    -- set the auto-generated config.h
    set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)")
    add_configfiles("xmake.config.h.in")

    -- add includes directory
    add_includedirs("..", {interface = true})
    add_includedirs("$(buildir)/$(plat)/$(arch)/$(mode)", {public = true})
    add_includedirs("../xxhash")
    add_includedirs("$(projectdir)/../xmake/scripts/module")

    -- add header files
    add_headerfiles("../(xmake/*.h)")
    add_headerfiles("../(xmake/prefix/*.h)")
    add_headerfiles("$(buildir)/$(plat)/$(arch)/$(mode)/xmake.config.h", {prefixdir = "xmake"})

    -- add the common source files
    add_files("**.c")

    -- add options
    add_options("readline")
    add_options("curses")
end)
