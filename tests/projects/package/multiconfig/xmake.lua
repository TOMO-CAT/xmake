add_requires("zlib", {system = false})
add_requires("zlib", {system = false}) -- test repeat requires
add_requires("zlib~debug", {system = false, debug = true})
add_requires("zlib~shared",
             {system = false, configs = {shared = true}, alias = "zlib_shared"})
-- set_policy("package.requires_lock", true)

target("test1", function()
    set_kind("binary")
    add_files("src/*.c")
    add_packages("zlib")
end)

target("test2", function()
    set_kind("binary")
    add_files("src/*.c")
    add_packages("zlib~debug")
end)

target("test3", function()
    set_kind("binary")
    add_files("src/*.c")
    add_packages("zlib_shared")
end)
