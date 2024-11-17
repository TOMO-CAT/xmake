add_rules("mode.debug", "mode.release")

set_languages("c++17")

local target_cnt = 300

target("jobpool_0", function()
    set_kind("object")
    add_files("src/1.cc")
    add_defines("TEST0")
end)

for i = 1, target_cnt do
    target("jobpool_" .. i, function()
        set_kind("object")
        add_files("src/1.cc")
        add_defines("TEST" .. tostring(i))
        add_deps("jobpool_" .. (i - 1))
    end)
end
