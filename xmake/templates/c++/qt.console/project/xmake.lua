add_rules("mode.debug", "mode.release")

target("${TARGETNAME}", function()
    add_rules("qt.console")
    add_files("src/*.cpp")
end)
