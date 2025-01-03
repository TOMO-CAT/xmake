
-- option: demo
option("demo")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the demo module")
option_end()

-- option: micro
option("micro")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("Compile micro core library for the embed system.")
    set_configvar("TB_CONFIG_MICRO_ENABLE", 1)
option_end()

-- option: small
option("small")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable the small compile mode and disable all modules.")
option_end()

-- option: wchar
option("wchar")
    add_ctypes("wchar_t")
    set_configvar("TB_CONFIG_TYPE_HAVE_WCHAR", 1)
option_end()

-- option: float
option("float")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the float type")
    add_deps("micro")
    set_configvar("TB_CONFIG_TYPE_HAVE_FLOAT", 1)
    after_check(function (option)
        if option:dep("micro"):enabled() then
            option:enable(false)
        end
    end)
option_end()

-- option: info
option("info")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable to get some info, .e.g version ..")
    add_deps("small", "micro")
    set_configvar("TB_CONFIG_INFO_HAVE_VERSION", 1)
    set_configvar("TB_CONFIG_INFO_TRACE_MORE", 1)
    after_check(function (option)
        if option:dep("small"):enabled() or option:dep("micro"):enabled() then
            option:enable(false)
        end
    end)
option_end()

-- option: exception
option("exception")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the exception.")
    set_configvar("TB_CONFIG_EXCEPTION_ENABLE", 1)
option_end()

-- option: deprecated
option("deprecated")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the deprecated interfaces.")
    set_configvar("TB_CONFIG_API_HAVE_DEPRECATED", 1)
option_end()

-- option: force-utf8
option("force-utf8")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("Force regard all tb_char* as utf-8.")
    set_configvar("TB_CONFIG_FORCE_UTF8", 1)
option_end()

-- add modules
for _, name in ipairs({"xml", "zip", "hash", "regex", "object", "charset", "database", "coroutine"}) do
    option(name)
        set_default(true)
        set_showmenu(true)
        set_category("module")
        set_description(format("The %s module", name))
        add_deps("small", "micro")
        set_configvar("TB_CONFIG_MODULE_HAVE_" .. name:upper(), 1)
        after_check(function (option)
            if option:dep("small"):enabled() or option:dep("micro"):enabled() then
                option:enable(false)
            end
        end)
    option_end()
end

-- include project directories
includes(format("tbox/%s.lua", (has_config("micro") and "micro" or "xmake")))
if has_config("demo") then
    includes(format("demo/%s.lua", (has_config("micro") and "micro" or "xmake")))
end
