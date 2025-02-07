-- add_requires("gtest v1.14.0")
-- 
-- for _, file in ipairs(os.files("**_test.c")) do
--     local name = path.join(path.directory(file), path.basename(file)):gsub("/",
--                                                                            ".")
--     target(name, function()
--         set_kind("binary")
--         set_default(false)
--         add_files(file)
--         add_tests("default")
--         add_packages("gtest")
--     end)
-- end
target("xutil-ut", function()
    add_deps("xutil")
    set_kind("binary")
    add_defines([[__xu_prefix__="xutil"]])
    add_files("**.c")

    -- link backtrace/execinfo for bsd
    if is_plat("bsd") then
        add_syslinks("execinfo")
    end
end)
