#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace Hyper {

struct IdtEntry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed));

struct Idtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct Idt {
    Idt();

    void setDescriptor(uint8_t vector, void *isr, uint8_t flags);

  private:
    __attribute__((aligned(0x10))) IdtEntry idt[256];
    Idtr idtr;
};

} // namespace Hyper

__attribute__((noreturn)) void exception_handler();

#ifdef __cplusplus
}
#endif