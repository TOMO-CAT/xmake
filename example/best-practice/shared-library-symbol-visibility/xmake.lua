add_includedirs("include")

target("foo-sdk-dynamic", function()
    set_kind("shared")
    set_filename("libfoosdk.so")
    add_files("src/*.cc")
    -- 检查动态库不缺符号
    add_shflags("-Wl,--no-undefined")
    -- 通过版本脚本仅导出公开的 SDK 接口，隐藏所有内部符号
    add_shflags("-Wl,--version-script=" .. os.projectdir() ..
                    "/exports/foo_sdk_private.map", {force = true})
end)

target("foo-sdk-package", function()
    set_kind("phony")
    add_installfiles("(include/**)")
end)
