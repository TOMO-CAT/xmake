rule("my-repo.check_test_target", function()
    on_config(function(target)
        local target_name = target:name()
        local target_kind = target:kind()
        local target_is_default = target:get("default")
        if target_kind == "binary" then
            -- binary target of unit-test
            if target:get("tests") or target_name:endswith("test") then
                if target_is_default == nil or target_is_default == true then
                    cprint(
                        "${bright yellow}[rule@cat/check_test_target][warning]${clear} set the `default` attribute of unit-test binary target [" ..
                            target_name .. "] to `false`")
                    target:set("default", false)
                end
            end
        end
    end)
end)
