add_rules("mode.debug", "mode.release")

target("cjson", function()
    add_rules("module.shared")
    set_warnings("all")

    add_files("src/*.c")
    add_files("../../../../../../../core/src/lua-cjson/lua-cjson/*.c|fpconv.c")
    -- Use internal strtod() / g_fmt() code for performance and disable multi-thread
    add_defines("NDEBUG", "USE_INTERNAL_FPCONV")
    add_defines("XM_CONFIG_API_HAVE_LUA_CJSON")
end)
