add_rules("mode.debug", "mode.release")

target("${TARGETNAME}", function()
    add_rules("qt.static")
    add_headerfiles("src/*.h")
    add_files("src/*.cpp")
    add_frameworks("QtGui")
end)
