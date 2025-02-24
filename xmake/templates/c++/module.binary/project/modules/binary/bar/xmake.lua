add_rules("mode.debug", "mode.release")

target("add", function()
    add_rules("module.binary")
    add_files("src/add.cpp")
end)

target("sub", function()
    add_rules("module.binary")
    add_files("src/sub.cpp")
end)
