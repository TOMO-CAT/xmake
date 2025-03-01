add_rules("mode.release", "mode.debug")

add_requires("cosmocc")
set_toolchains("@cosmocc")

target("foo", function()
    set_kind("static")
    add_files("src/foo.c")
end)

target("demo", function()
    set_kind("binary")
    add_deps("foo")
    add_files("src/main.c")
end)
