includes("toolchain.lua")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
add_rules("mode.debug", "mode.release")

add_cxxflags("-Wall", "-Wextra", "-Werror")
add_cxxflags("-Wno-unused-parameter")

set_languages("c++17")

-- 这种修改 clang 编译参数的必须要依赖本地 protoc
-- sudo apt install protobuf-compiler

-- 编译出 cross 版本的 protobuf 库, 用于构造交叉编译成品库
-- 注意 protobuf 版本要和 protoc 版本一致: 3.12.4
package("protobuf-cpp", function()

    set_homepage("https://developers.google.com/protocol-buffers/")
    set_description("Google's data interchange format for cpp")

    add_urls(
        "https://github.com/protocolbuffers/protobuf/releases/download/v$(version)",
        {
            version = function(version)
                return version .. "/protobuf-cpp-" .. version .. ".zip"
            end
        })

    add_versions("3.12.4",
                 "5ad4cf085cfd866043dc1035c8f8e97e6968573a2d117e054e2a890eb19259d1")

    add_deps("cmake")

    add_links("protobuf", "protoc", "utf8_range", "utf8_validity")

    if is_plat("linux") then
        add_syslinks("pthread")
    end

    on_load(function(package)
        package:addenv("PATH", "bin")
    end)

    on_install(function(package)
        os.cd("cmake")
        io.replace("CMakeLists.txt", "set(protobuf_DEBUG_POSTFIX \"d\"",
                   "set(protobuf_DEBUG_POSTFIX \"\"", {plain = true})

        local configs = {
            "-Dprotobuf_BUILD_TESTS=OFF", "-Dprotobuf_BUILD_PROTOC_BINARIES=ON"
        }
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" ..
                         (package:is_debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" ..
                         (package:config("shared") and "ON" or "OFF"))

        local packagedeps = {}

        import("package.tools.cmake").install(package, configs, {
            buildir = "build",
            packagedeps = packagedeps
        })
    end)

    on_test(function(package)
        if package:is_cross() then
            return
        end
        io.writefile("test.proto", [[
            syntax = "proto3";
            package test;
            message TestCase {
                string name = 4;
            }
            message Test {
                repeated TestCase case = 1;
            }
        ]])
        os.vrun("protoc test.proto --cpp_out=.")
    end)
end)
add_requires("protobuf-cpp 3.12.4")

target("proto", function()
    set_kind("object")
    add_files("pb/*.proto", {proto_public = true})
    add_rules("protobuf.cpp")
    add_packages("protobuf-cpp", {public = true})
    set_policy('build.fence', true)
end)

target("clang-sysroot-target", function()
    set_kind("binary")
    add_files("src/*.cc")
    add_deps("proto")
end)
