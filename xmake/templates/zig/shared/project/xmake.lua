add_rules("mode.debug", "mode.release")

target("${TARGETNAME}", function()
    set_kind("shared")
    add_files("src/test.zig")
end)

target("${TARGETNAME}_demo", function()
    set_kind("binary")
    add_deps("${TARGETNAME}")
    add_files("src/main.zig")
end)
