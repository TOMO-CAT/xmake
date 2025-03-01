add_rules("mode.debug", "mode.release")

target("${TARGETNAME}", function()
    set_kind("shared")
    add_files("src/interfaces.d")
    add_includedirs("src", {public = true})
    add_rules("utils.symbols.export_list", {symbols = {
      "add",
      "sub"}})
end)

target("${TARGETNAME}_demo", function()
    set_kind("binary")
    add_deps("${TARGETNAME}")
    add_files("src/main.d")
end)
