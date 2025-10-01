add_requires("zlib >=1.2.11")
add_requires("pcre2", {system = false, optional = true})

add_rules("mode.debug", "mode.release")

target("test", function()
    set_kind("static")
    add_files("src/test.c")
    add_packages("pcre2", {public = true})
end)

target("console", function()
    set_kind("binary")
    add_deps("test")
    add_files("src/main.c")
    add_packages("zlib")
end)
