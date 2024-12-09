includes("../packages.lua")

add_rules("mode.debug", "mode.release")

add_requires("xproto", "xcore")

target("xmath", function()
    set_kind("static")
    add_files("xmath.cc")
end)
