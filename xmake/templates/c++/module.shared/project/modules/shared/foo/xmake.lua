add_rules("mode.debug", "mode.release")

target("foo", function()
    add_rules("module.shared")
    add_files("src/foo.cpp")
end)
