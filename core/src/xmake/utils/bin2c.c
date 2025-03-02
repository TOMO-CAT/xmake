/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "bin2c"
#define XU_TRACE_MODULE_DEBUG (0)

/* *******************************************************
 * includes
 */
#include "xmake/prefix.h"

/* *******************************************************
 * private implementation
 */
static __xu_inline__ xu_size_t xm_utils_bin2c_hex2str(xu_char_t str[5], xu_byte_t value)
{
    static xu_char_t const* digits_table = "0123456789ABCDEF";
    str[0]                               = ' ';
    str[1]                               = '0';
    str[2]                               = 'x';
    str[3]                               = digits_table[(value >> 4) & 15];
    str[4]                               = digits_table[value & 15];
    return 5;
}

static xu_bool_t xm_utils_bin2c_dump(xu_stream_ref_t istream, xu_stream_ref_t ostream, xu_int_t linewidth,
                                     xu_bool_t nozeroend)
{
    xu_bool_t first = xu_true;
    xu_hong_t i     = 0;
    xu_hong_t left  = 0;
    xu_char_t line[4096];
    xu_byte_t data[512];
    xu_size_t linesize = 0;
    xu_size_t need     = 0;
    xu_assert_and_check_return_val(linewidth < sizeof(data), xu_false);
    while (!xu_stream_beof(istream))
    {
        linesize = 0;
        left     = xu_stream_left(istream);
        need     = (xu_size_t)xu_min(left, linewidth);
        if (need)
        {
            if (!xu_stream_bread(istream, data, need)) break;

            if (!nozeroend && xu_stream_beof(istream))
            {
                xu_assert_and_check_break(need + 1 < sizeof(data));
                data[need++] = '\0';
            }

            xu_assert_and_check_break(linesize + 6 * need < sizeof(line));

            i = 0;
            if (first)
            {
                first            = xu_false;
                line[linesize++] = ' ';
            }
            else
                line[linesize++] = ',';
            linesize += xm_utils_bin2c_hex2str(line + linesize, data[i]);

            for (i = 1; i < need; i++)
            {
                line[linesize++] = ',';
                linesize += xm_utils_bin2c_hex2str(line + linesize, data[i]);
            }
            xu_assert_and_check_break(i == need && linesize && linesize < sizeof(line));

            if (xu_stream_bwrit_line(ostream, line, linesize) < 0) break;
        }
    }

    return xu_stream_beof(istream);
}

/* *******************************************************
 * implementation
 */
/* generate c/c++ code from the binary file
 *
 * local ok, errors = utils.bin2c(binaryfile, outputfile, linewidth, nozeroend)
 */
xu_int_t xm_utils_bin2c(lua_State* lua)
{
    // check
    xu_assert_and_check_return_val(lua, 0);

    // get the binaryfile
    xu_char_t const* binaryfile = luaL_checkstring(lua, 1);
    xu_check_return_val(binaryfile, 0);

    // get the outputfile
    xu_char_t const* outputfile = luaL_checkstring(lua, 2);
    xu_check_return_val(outputfile, 0);

    // get line width
    xu_int_t linewidth = (xu_int_t)lua_tointeger(lua, 3);

    // no zero end?
    xu_bool_t nozeroend = (xu_bool_t)lua_toboolean(lua, 4);

    // do dump
    xu_bool_t       ok      = xu_false;
    xu_stream_ref_t istream = xu_stream_init_from_file(binaryfile, XU_FILE_MODE_RO);
    xu_stream_ref_t ostream =
        xu_stream_init_from_file(outputfile, XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);
    do
    {
        if (!xu_stream_open(istream))
        {
            lua_pushboolean(lua, xu_false);
            lua_pushfstring(lua, "bin2c: open %s failed", binaryfile);
            break;
        }

        if (!xu_stream_open(ostream))
        {
            lua_pushboolean(lua, xu_false);
            lua_pushfstring(lua, "bin2c: open %s failed", outputfile);
            break;
        }

        if (!xm_utils_bin2c_dump(istream, ostream, linewidth, nozeroend))
        {
            lua_pushboolean(lua, xu_false);
            lua_pushfstring(lua, "bin2c: dump data failed");
            break;
        }

        ok = xu_true;
        lua_pushboolean(lua, ok);

    } while (0);

    if (istream) xu_stream_clos(istream);
    istream = xu_null;

    if (ostream) xu_stream_clos(ostream);
    ostream = xu_null;

    return ok ? 1 : 2;
}
