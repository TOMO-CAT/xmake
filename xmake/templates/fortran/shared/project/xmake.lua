add_rules("mode.debug", "mode.release")

target("${TARGETNAME}_lib", function()
    set_kind("shared")
    add_files("src/test.f90")
end)

target("${TARGETNAME}", function()
    set_kind("binary")
    add_deps("${TARGETNAME}_lib")
    add_files("src/main.f90")
end)
