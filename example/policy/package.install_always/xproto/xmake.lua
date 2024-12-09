add_rules("mode.debug", "mode.release")

target("xproto", function()
    set_kind("static")
    add_files("xproto.cc")
end)
