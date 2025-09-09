-- @see https://github.com/TOMO-CAT/xmake/issues/241
add_requires("tbox v1.7.7", {system = false, configs = {shared = false}})

target("main", function()
    set_kind("binary")
    add_files("main.cc")
    add_packages("tbox")
    add_linkgroups("tbox", {whole = true})
end)
