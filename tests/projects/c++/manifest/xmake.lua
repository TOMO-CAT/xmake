add_rules("mode.debug", "mode.release")

target("test1", function()
    set_kind("binary")
    add_files("src/*.cpp")
    add_files("src/*.manifest")
end)

target("test2", function()
    set_kind("binary")
    add_files("src/*.cpp")
end)
