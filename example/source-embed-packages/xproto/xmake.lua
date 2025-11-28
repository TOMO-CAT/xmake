add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_cxxflags("-Wall", "-Wextra", "-Werror")

add_requires("protobuf-cpp 3.19.4", {system = false})

target("xproto", function()
    set_kind("static")
    add_files("common/*.proto", {proto_public = true, proto_rootdir = "."})
    add_rules("protobuf.cpp")
    add_packages("protobuf-cpp", {public = true})
    on_load(function(target)
        target:add("headerfiles", path.join(target:autogendir(), "rules",
                                            "protobuf", "(**.h)"))
    end)
    set_policy('build.fence', true)
end)
