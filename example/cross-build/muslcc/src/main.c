#include <stdio.h>
#include <stdint.h>
#include <zlib.h>
#include <ogg/ogg.h>

int main(int argc, char** argv)
{
    printf("hello world!\n");
    inflate(0, 0);
    ogg_sync_init(0);
    return 0;
}
