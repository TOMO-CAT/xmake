template("tbox.shared", function()
    add_configfiles("xmake.lua")
    add_configfiles("src/_demo/main.c")
    add_configfiles("src/_demo/xmake.lua")
    add_configfiles("src/_library/xmake.lua")
    after_create(function (template, opt)
        os.mv("src/_library", path.join("src", opt.targetname))
        os.mv("src/_demo", path.join("src", opt.targetname .. "_demo"))
    end)
end)
