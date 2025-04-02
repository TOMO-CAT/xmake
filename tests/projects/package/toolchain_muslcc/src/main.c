#include <stdio.h>
#include <stdint.h>
#include <zlib.h>
#ifdef HAVE_LIBPLIST
#   include <plist/plist.h>
#endif

int main(int argc, char** argv)
{
    printf("hello world!\n");
    inflate(0, 0);
#ifdef HAVE_LIBPLIST
    plist_new_dict();
#endif
    return 0;
}
