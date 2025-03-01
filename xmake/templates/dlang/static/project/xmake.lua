add_rules("mode.debug", "mode.release")

target("${TARGETNAME}", function()
    set_kind("static")
    add_files("src/interfaces.d")
    add_includedirs("src", {public = true})
end)

target("${TARGETNAME}_demo", function()
    set_kind("binary")
    add_deps("${TARGETNAME}")
    add_files("src/main.d")
end)
