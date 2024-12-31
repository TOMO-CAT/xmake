target("demo", function()

    -- disable this target if only build libaries
    if has_config("onlylib") then
        set_default(false)
    end

    -- add deps
    add_deps("xmake")

    -- make as a binary
    set_kind("binary")
    set_basename("xmake")
    set_targetdir("$(buildir)")

    -- add definitions
    add_defines("__tb_prefix__=\"xmake\"")

    -- add includes directory
    add_includedirs("$(projectdir)", "$(projectdir)/src")

    -- add common source files
    add_files("**.c")

    -- add links
    if is_plat("android") then
        add_syslinks("m", "c")
    elseif is_plat("macosx") and is_config("runtime", "luajit") then
        add_ldflags("-all_load", "-pagezero_size 10000", "-image_base 100000000")
    elseif is_plat("haiku") then
        add_syslinks("pthread", "network", "m", "c")
    else
        add_syslinks("pthread", "dl", "m", "c")
    end

    -- add install files
    add_installfiles("$(projectdir)/../(xmake/**.lua)", {prefixdir = "share"})
    add_installfiles("$(projectdir)/../(xmake/scripts/**)", {prefixdir = "share"})
    add_installfiles("$(projectdir)/../(xmake/templates/**)", {prefixdir = "share"})
    add_installfiles("$(projectdir)/../scripts/xrepo.sh", {prefixdir = "bin", filename = "xrepo"})
end)
