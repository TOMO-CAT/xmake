add_rules("mode.release")
set_languages("c++17")

add_requires("protobuf-cpp 3.19.4")

target("add_specific_version_package", function()
    set_kind("binary")
    add_files("src/test_pb.cc")
    add_packages("protobuf-cpp")

    -- 错误用法, 不能在这里指定版本
    -- add_packages("protobuf-cpp 3.19.4")
end)
