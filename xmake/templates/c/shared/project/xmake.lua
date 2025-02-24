add_rules("mode.debug", "mode.release")

target("foo", function()
    set_kind("shared")
    add_files("src/foo.c")
end)

target("${TARGETNAME}", function()
    set_kind("binary")
    add_deps("foo")
    add_files("src/main.c")
end)
