add_rules("mode.debug", "mode.release")

target("${TARGETNAME}", function()
    add_rules("qt.quickapp")
    add_headerfiles("src/*.h")
    add_files("src/*.cpp")
    add_files("src/qml.qrc")
end)
