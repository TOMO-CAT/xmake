add_requires("libpng", {system = false, configs = {runtimes = "MD"}})

add_requireconfs("libpng.zlib", {
    system = false,
    override = true,
    configs = {cxflags = "-DTEST1"},
    version = "1.3.1"
})

target("test", function()
    set_kind("binary")
    add_files("src/*.c")
    add_packages("libpng")
    before_build(function(target)
        if target:pkg("libpng") then
            local found
            for _, linkdir in ipairs(target:pkg("libpng"):get("linkdirs")) do
                if linkdir:find("zlib[/\\]v1%.3%.1[/\\]") then
                    found = true
                end
            end
            assert(found, "package(zlib 1.3.1) not found!")
        end
    end)
end)
