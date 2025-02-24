-- add modes: debug and release
add_rules("mode.debug", "mode.release")

-- add target
target("${TARGETNAME}", function()

    -- set kind
    set_kind("binary")

    -- add files
    add_files("src/*.mm")

end)
