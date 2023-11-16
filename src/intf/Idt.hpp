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
    void install();
    void setDescriptor(uint8_t vector, void *isr, uint8_t flags);

  private:
    bool vectors[32];
    __attribute__((aligned(0x10))) IdtEntry idt[256];
    Idtr idtr;
};

} // namespace Hyper

constexpr const char *exception_messages[] = {
    "Divide by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 floating-point exception",
    "Alignment check",
    "Machine check",
    "SIMD floating-point exception",
    "Virtualization exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security exception",
    "Reserved",
    "Triple fault",
};

__attribute__((noreturn)) void
exception_handler(uint64_t vector, uint64_t error_code,
                  uint64_t instruction_pointer, uint64_t code_segment,
                  uint64_t cpu_flags, uint64_t stack_pointer,
                  uint64_t stack_segment);

#ifdef __cplusplus
}
#endif