includes("packages.lua")

add_rules("mode.debug", "mode.release")

add_requires("xproto", "xcore", "xmath")

target("demo", function()
    set_kind("binary")
    add_files("main.cc")
    add_packages("xproto", "xcore")
end)
