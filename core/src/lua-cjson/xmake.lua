target("lua-cjson", function()
    set_kind("static")
    set_warnings("none") -- ignore warnings
    if is_config("runtime", "luajit") then
        add_deps("luajit")
    else
        add_deps("lua")
    end
    add_files("lua-cjson/*.c|fpconv.c")
    -- Use internal strtod() / g_fmt() code for performance and disable multi-thread
    add_defines("NDEBUG", "USE_INTERNAL_FPCONV")
    add_defines("XM_CONFIG_API_HAVE_LUA_CJSON", {public = true})
end)
