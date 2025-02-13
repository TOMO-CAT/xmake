/* *******************************************************
 * includes
 */
#include "xmake/xmake.h"

/* *******************************************************
 * main
 */
xu_int_t main(xu_int_t argc, xu_char_t** argv)
{
    return xm_engine_run("xmake", argc, argv, xu_null, xu_null);
}
