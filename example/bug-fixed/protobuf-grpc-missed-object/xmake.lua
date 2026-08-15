add_rules("mode.debug", "mode.release")

set_languages("c++17")

target("proto", function()
    set_kind("object")
    add_rules("protobuf.cpp")
    add_files("src/service.proto", {
        proto_public = true,
        proto_rootdir = "src",
        proto_grpc_cpp_plugin = true
    })
    set_policy("build.fence", true)
end)

target("grpc", function()
    set_kind("binary")
    add_files("src/main.cpp")
    add_deps("proto")
    add_syslinks("protobuf", "grpc++")
end)
