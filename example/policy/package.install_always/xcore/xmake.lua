includes("../packages.lua")

add_rules("mode.debug", "mode.release")

add_requires("xproto")

target("xcore", function()
    set_kind("static")
    add_files("xcore.cc")
    add_packages("xproto")
end)
