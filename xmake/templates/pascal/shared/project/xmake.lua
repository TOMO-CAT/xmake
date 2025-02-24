add_rules("mode.debug", "mode.release")

target("foo", function()
    set_kind("shared")
    add_files("src/foo.pas")
end)

target("${TARGETNAME}", function()
    set_kind("binary")
    add_deps("foo")
    add_files("src/main.pas")
end)
