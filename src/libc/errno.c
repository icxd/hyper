#include <errno.h>

int *__errno_location(void) {
    static int e;
    return &e;
}