add_rules("mode.debug", "mode.release")

set_languages("c++17")

add_requires("protobuf-cpp 3.19.4")

target("proto", function()
    set_kind("object")
    add_files("proto/*.proto", {
        proto_public = true,
        proto_rootdir = ".",
    })
    add_rules("protobuf.cpp")
    add_packages("protobuf-cpp", {public = true})
    set_policy('build.fence', true)
end)
