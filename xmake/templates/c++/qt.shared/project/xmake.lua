add_rules("mode.debug", "mode.release")

target("${TARGETNAME}", function()
    add_rules("qt.shared")
    add_headerfiles("src/*.h")
    add_files("src/*.cpp")
    add_defines("QT_DEMO_LIBRARY")
    add_frameworks("QtGui")
end)
