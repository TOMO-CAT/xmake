add_rules("mode.debug", "mode.release")

target("test", function()
    add_deps("foo")
    set_kind("binary")
    add_files("src/main.c")
    if is_plat("linux") and is_arch("x86_64") then
        add_files("src/main.lds")
    end
end)

target("foo", function()
    set_kind("shared")
    add_files("src/foo.c")
    add_files("src/foo.map")
end)
