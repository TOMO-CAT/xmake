add_rules("mode.release", "mode.debug")

target("foo")
    set_kind("shared")
    add_files("src/foo.c")

target("test")
    set_kind("binary")
    add_deps("foo")
    add_files("src/main.c")
