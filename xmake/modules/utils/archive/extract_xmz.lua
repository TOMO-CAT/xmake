-- imports
import("core.base.option")
import("core.base.bytes")
import("core.compress.lz4")

-- extract files
function _extract_files(archivefile, outputdir, opt)
    local inputfile = io.open(archivefile, "rb")
    local filesize = inputfile:size()
    local readsize = 0
    while readsize < filesize do
        local data = inputfile:read(2)
        local size = bytes(data):u16be(1)
        local filepath
        if size > 0 then
            filepath = inputfile:read(size)
        end
        readsize = readsize + 2 + size
        data = inputfile:read(4)
        size = bytes(data):u32be(1)
        local filedata
        if size > 0 then
            filedata = inputfile:read(size)
        end
        readsize = readsize + 4 + size
        if filepath then
            vprint("extracting %s, %d bytes", filepath,
                   filedata and #filedata or 0)
            if filedata then
                io.writefile(path.join(outputdir, filepath), filedata,
                             {encoding = "binary"})
            else
                os.touch(filepath)
            end
        end
    end
    inputfile:close()
end

-- decompress file
function _decompress_file(archivefile, outputfile, opt)
    lz4.decompress_file(archivefile, outputfile)
end

-- extract file
--
-- @param archivefile   the archive file. e.g. *.tar.gz, *.zip, *.7z, *.tar.bz2, ..
-- @param outputdir     the output directory
-- @param options       the options, e.g.. {excludes = {"*/dir/*", "dir/*"}}
--
function main(archivefile, outputdir, opt)
    opt = opt or {}

    local archivefile_tmp = os.tmpfile({ramdisk = false})
    _decompress_file(archivefile, archivefile_tmp, opt)
    _extract_files(archivefile_tmp, outputdir, opt)
    os.tryrm(archivefile_tmp)
end
