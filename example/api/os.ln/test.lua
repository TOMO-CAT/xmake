-- xmake l example/api/os.ln/test.lua
function main(opt)
    os.ln(path.join(os.scriptdir(),"target.txt"), path.join(os.scriptdir(), "target.txt.ln"))
end
