-- @see https://github.com/TOMO-CAT/xmake/issues/241
add_requires("zlib v1.2.10", {system = false, configs = {shared = false}})

target("main", function()
    set_kind("binary")
    add_files("main.cc")
    add_packages("zlib", {links = {}})
    add_linkgroups("z", {whole = true})
end)
