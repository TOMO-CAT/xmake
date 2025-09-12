add_requires("zlib v1.2.10", {system = false})
add_requires("zlib v1.2.13", {system = false})
add_requires("zlib v1.3", {system = false})
add_requires("zlib v1.3.1", {system = false})

-- 内部会依赖 zlib
add_requires("ixwebsocket v11.4.5", {system = false})
add_requires("gzip-hpp v0.1.0", {system = false})

-- 人为制造版本相同
add_requireconfs("ixwebsocket.zlib", {override = true, version = "v1.3.1", system = false})
add_requireconfs("gzip-hpp.zlib", {override = true, version = "v1.3.1", system = false})

target("main", function()
    set_kind("binary")
    add_files("main.cc")
    add_packages("zlib")
end)
