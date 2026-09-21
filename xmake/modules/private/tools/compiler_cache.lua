-- imports
import("private.tools.ccache")
import("private.tools.sccache")

-- get the preferred compiler cache tool
function get(target)
    if sccache.is_enabled(target) then
        return sccache, "sccache"
    elseif ccache.is_enabled(target) then
        return ccache, "ccache"
    end
end

-- dump compiler cache statistics
function dump_stats(target)
    local cache_tool = get(target)
    if cache_tool and cache_tool.dump_stats then
        cache_tool.dump_stats()
    end
end
