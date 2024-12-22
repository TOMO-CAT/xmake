add_rules("mode.debug", "mode.release")

target("zig-cross", function()
    set_kind("binary")
    add_files("src/*.cc")
end)
