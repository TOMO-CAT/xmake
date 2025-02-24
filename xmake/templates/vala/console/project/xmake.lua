add_rules("mode.debug", "mode.release")

add_requires("lua", "glib")

target("${TARGETNAME}", function()
    set_kind("binary")
    add_rules("vala")
    add_files("src/*.vala")
    add_packages("lua", "glib")
    add_values("vala.packages", "lua")
end)
