add_rules("mode.debug", "mode.release")

target("module", function()
    set_kind("static")
    add_files("src/test/*.go")
end)

target("${TARGETNAME}_demo", function()
    set_kind("binary")
    add_deps("module")
    add_files("src/*.go")
end)
