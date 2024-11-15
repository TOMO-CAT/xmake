add_rules("mode.debug", "mode.release")
set_languages("c++17")
add_includedirs(".")

target("normal.1", function()
    set_kind("object")
    add_files("src/1.cc")
end)

for i = 2, 50 do
    target("normal." .. tostring(i), function()
        set_kind("object")
        add_files(path.join("src", tostring(i) .. ".cc"))
        add_deps("normal." .. tostring(i - 1))
    end)
end

-- 依赖树末端的 target, 正常来说应该是在最后编译
target("high_priority", function()
    set_kind("object")
    add_files("src/high_priority.cc")
    add_deps("normal.50")

    -- 通过这个策略让其尽可能前置编译 (除非其子 target 设置了 build.fence 为 true)
    set_policy("build.high_priority", true)
end)
