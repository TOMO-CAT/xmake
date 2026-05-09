add_rules("mode.debug", "mode.release")

set_version("1.0.1", {soname = true})

target("foo", function()
    set_kind("shared")
    add_files("src/foo.cpp")
    after_build(function (target)
        print(target:soname())
    end)
end)

target("tests", function()
    set_kind("binary")
    add_deps("foo")
    add_files("src/main.cpp")
end)
