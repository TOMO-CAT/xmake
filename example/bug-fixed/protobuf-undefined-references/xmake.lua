add_rules("mode.debug", "mode.release")
add_requires("protobuf-cpp 3.19.4")
set_languages("c++latest")

target("empty", function()
    set_kind("object")
end)

target("a",  function()
    set_kind("object")
    add_deps("empty")
    add_packages("protobuf-cpp")
    add_rules("protobuf.cpp")
    add_files("src/a.cpp")
    add_files("src/a.proto", {proto_rootdir = "src"})
end)

target("b", function()
    set_kind("binary")
    add_deps("a")
    add_packages("protobuf-cpp")
    add_rules("protobuf.cpp")
    add_files("src/b.cpp")
    add_files("src/b.proto", {proto_rootdir = "src"})
end)
