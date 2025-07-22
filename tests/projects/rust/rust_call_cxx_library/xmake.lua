add_rules("mode.debug", "mode.release")

target("foo", function()
    set_kind("static")
    add_files("src/foo.cc")
end)

target("test", function()
    set_kind("binary")
    add_deps("foo")
    add_files("src/main.rs")
end)
