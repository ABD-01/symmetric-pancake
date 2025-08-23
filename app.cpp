#include <stdint.h>
#include "shell.h"

#include "app.h"


static int g_xyz = 42;

int foo()
{
    bar(g_xyz);
    return 0;
}
void bar(int arg)
{
    int i = arg + 64;
    int j = baz(i);
    (void)j;
    return;
}

int baz(int arg)
{
    if (arg > 32 && arg < 100) {
        return 78;
    } else {
        volatile uint32_t *bat_ptr = (uint32_t *)0xDEADBEEF;
        *bat_ptr                   = 0x12345678;
        return 9;
    }
}

ADD_CMD(foo, "Calls function foo->bar->baz which segfaults", foo);