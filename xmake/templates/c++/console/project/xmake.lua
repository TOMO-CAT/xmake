add_rules("mode.debug", "mode.release")

target("${TARGETNAME}", function()
    set_kind("binary")
    add_files("src/*.cpp")
end)
