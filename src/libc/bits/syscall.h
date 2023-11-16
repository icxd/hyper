#pragma once

#ifndef _SYSCALL_H
#error "Never use <bits/syscall.h> directly; include <sys/syscall.h> instead."
#endif

#include <sys/cdefs.h>

__BEGIN_DECLS

enum {
    SYS_read = 0x00,
    SYS_write,
    SYS_open,
    SYS_close,
};

__END_DECLS