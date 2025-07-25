add_rules("mode.debug", "mode.release")

-- MacOS 默认装了 libpng, 可能会报错
add_requires("libpng", {system = false})

target("bar", function()
    set_kind("shared")
    add_files("src/foo.cpp")
    add_linkgroups("m", "pthread", {whole = true})
end)

target("foo", function()
    set_kind("static")
    add_files("src/foo.cpp")
    add_packages("libpng", {public = true})
end)

target("demo", function()
    set_kind("binary")
    add_deps("foo")
    add_files("src/main.cpp")
    if is_plat("linux", "macosx") then
        add_syslinks("pthread", "m", "dl")
    end
    if is_plat("macosx") then
        add_frameworks("Foundation", "CoreFoundation")
    end
    add_linkorders("framework::Foundation", "png16", "foo")
    add_linkorders("dl", "linkgroup::syslib")
    add_linkgroups("m", "pthread", {name = "syslib", group = true})
end)
