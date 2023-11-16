#pragma once

#ifdef __cplusplus
extern "C" {
#endif

namespace Hyper {

#define CAT_HELPER(x, y) x##y
#define CAT(x, y) CAT_HELPER(x, y)

#define PANIC(msg, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        asm volatile("cli");                                                   \
        asm volatile("hlt");                                                   \
    } while (0)

} // namespace Hyper

#ifdef __cplusplus
}
#endif