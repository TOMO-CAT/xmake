rule("my-repo.mode.release", function()
    on_config(function(target)
        if is_mode("release") then
            -- cuda
            target:add("cuflags", "-DNDEBUG", {force = true})
            target:add("culdflags", "-DNDEBUG", {force = true})

            -- c & cpp
            target:add("cxflags", "-fPIC")
            if is_arch("arm.*") then
                target:add("cxflags", "-g")
                target:add("cxflags", "-gz", "-gdwarf-4")
                target:add("cxflags", "-DNDEBUG")
            end

            if not target:get("optimize") then
                target:set("optimize", "fastest")
            end
        end
    end)
end)
