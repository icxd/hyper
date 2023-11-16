#include <errno.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <unistd.h>

long syscall(long sysno, ...) {
    va_list ap;
    va_start(ap, sysno);

    void *arg1 = va_arg(ap, void *);
    void *arg2 = va_arg(ap, void *);
    void *arg3 = va_arg(ap, void *);
    void *arg4 = va_arg(ap, void *);
    void *arg5 = va_arg(ap, void *);
    void *arg6 = va_arg(ap, void *);

    va_end(ap);

    long ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(sysno), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4),
                   "D"(arg5), "r"(arg6)
                 : "memory", "cc");
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    return ret;
}