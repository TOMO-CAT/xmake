-- 单测 target
-- @see https://github.com/TOMO-CAT/xmake/issues/210

rule("unittest", function()
    on_config(function(target)
        if target:kind() ~= "test" then
            return
        end
    
        target:set("kind", "binary")
        target:set("default", false)
        if not target:get("rundir") then
            target:set("rundir", os.projectdir())
        end
        if not target:get("tests") then
            target:add("tests", "default")
        end
    end)
end)
