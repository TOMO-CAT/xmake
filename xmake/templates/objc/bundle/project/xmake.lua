add_rules("mode.debug", "mode.release")

target("${TARGETNAME}", function()
    add_rules("xcode.bundle")
    add_files("src/*.m")
    add_files("src/Info.plist")
    add_headerfiles("src/*.h")
end)
