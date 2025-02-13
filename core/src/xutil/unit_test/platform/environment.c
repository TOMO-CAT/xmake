#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_environment_main(xu_int_t argc, xu_char_t** argv)
{
    // init environment
    xu_environment_ref_t environment = xu_environment_init();
    if (environment)
    {
        // load variable
        if (xu_environment_load(environment, argv[1]))
        {
            // dump it
#ifdef __xu_debug__
            xu_environment_dump(environment, argv[1]);
#endif
        }

        // save variable?
        if (argc > 2)
        {
            xu_size_t i = 0;
            for (i = 2; i < argc && argv[i]; i++)
            {
                // set value
                xu_environment_insert(environment, argv[i], xu_false);
            }

            // save variable
            xu_environment_save(environment, argv[1]);

            // load variable
            if (xu_environment_load(environment, argv[1]))
            {
                // dump it
#ifdef __xu_debug__
                xu_environment_dump(environment, argv[1]);
#endif
            }
        }

        // exit environment
        xu_environment_exit(environment);
    }
    return 0;
}
