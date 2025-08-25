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
        volatile uint32_t *bat_ptr = (uint32_t *)0xDEADBEE0;
        *bat_ptr                   = 0x12345678;
        return 9;
    }
}

void iaccviol(void) {
  void (*bad_func)(void) = (void *)0xEEEEDEAD;
  bad_func();
  return;
}

void divide_by_zero(void) {
  volatile int a = 1;
  volatile int b = 0;
  volatile int c = a / b;
  (void)c;
  return;
}

void unaligned_access(void) {
  uint32_t val = 0x12345678;
  uint8_t *ptr = (uint8_t *)&val;
  // Force unaligned access
  uint16_t *uptr = (uint16_t *)(ptr + 1);
  volatile uint16_t read_val = *uptr;
  (void)read_val;
  return;
}

ADD_CMD(foo, "Calls function foo->bar->baz which segfaults", foo);
ADD_CMD(bad, "Calls invalid function pointer to demonstrate fault", iaccviol);
ADD_CMD(div0, "Divides by zero to demonstrate fault", divide_by_zero);
ADD_CMD(una, "Access your secret without alignment", unaligned_access);
