add_rules("mode.debug", "mode.release")

target("interfaces", function()
    set_kind("static")
    add_files("src/interfaces.d")
    add_includedirs("src", {public = true})
end)

target("test", function()
    set_kind("binary")
    add_deps("interfaces")
    add_files("src/main.d")
end)
