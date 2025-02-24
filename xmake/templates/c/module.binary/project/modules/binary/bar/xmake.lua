add_rules("mode.debug", "mode.release")

target("add", function()
    add_rules("module.binary")
    add_files("src/add.c")
end)

target("sub", function()
    add_rules("module.binary")
    add_files("src/sub.c")
end)
