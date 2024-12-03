add_rules("mode.debug", "mode.release")
add_requires("libxmake")

target("xmake", function()
    add_rules("xmake.cli")
    add_files("src/*.c")
    add_packages("libxmake")
end)
