#include <Qemu.hpp>
#include <stdarg.h>
#include <stdio.h>

extern Hyper::Qemu qemu;

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    qemu.svprintf(fmt, args);

    va_end(args);
    return 0;
}