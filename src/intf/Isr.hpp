#pragma once

#include <Idt.hpp>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// using Stub = void *(*)(void);
// extern Stub isr_stub_table[256];
extern void *isr_stub_table[256];

namespace Hyper {

struct Registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

typedef void (*IsrHandler)(Registers *);

struct Isr {
    static void isrInstall(Idt *idt);
    static void irqInstall();
    void isrHandler(Registers *registers);

    void registerHandler(uint8_t n, IsrHandler handler);

  private:
    IsrHandler handlers[256];
};

} // namespace Hyper

#ifdef __cplusplus
}
#endif