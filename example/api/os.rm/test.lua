-- xmake l example/api/os.rm/test.lua
function main(opt)
    local scriptdir = os.scriptdir()
    os.cd(scriptdir)

    -- create a directory to delete
    os.mkdir("test-folder")
    os.mkdir("test-folder/test-subfolder")
    io.writefile("test-folder/test-file.txt", "tomo")

    -- create a folder softlink && a file softlink
    os.ln(path.absolute("my-folder"), path.absolute("test-folder/my-folder.ln"))
    os.ln(path.absolute("my-folder/my-file.txt"), path.absolute("test-folder/my-file.txt.ln"))

    -- it should not delete any files in my-folder
    -- os.tryrm("test-folder")
    os.rm("test-folder/my-folder.ln")
    os.rm("test-folder")
    assert(os.exists("my-folder/my-file.txt"))
end
