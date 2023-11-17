#include <Idt.hpp>
#include <Qemu.hpp>

#define GDT_OFFSET_KERNEL_CODE 0x08 // most likely invalid, double check later
#define IDT_MAX_DESCRIPTORS 256

extern void *isr_stub_table[];

extern Hyper::Qemu qemu;

namespace Hyper {

void Idt::install() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(IdtEntry) * IDT_MAX_DESCRIPTORS - 1;

    // for (uint8_t vector = 0; vector < IDT_MAX_DESCRIPTORS; vector++) {
    //     setDescriptor(vector, isr_stub_table[vector], 0x8e);
    //     vectors[vector] = true;
    // }

    asm volatile("lidt %0" : : "m"(idtr)); // load new idt
    asm volatile("sti");                   // set the interrupt flag
}

void Idt::setDescriptor(uint8_t vector, void *isr, uint8_t flags) {
    IdtEntry *descriptor = &idt[vector];

    descriptor->isr_low = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved = 0;
}

} // namespace Hyper

void exception_handler(uint64_t vector, uint64_t instruction_pointer,
                       uint64_t code_segment, uint64_t cpu_flags,
                       uint64_t stack_pointer, uint64_t stack_segment,
                       uint64_t error_code) {
    qemu.printf("Exception handler called: 0x%X (%s)\n", vector,
                exception_messages[vector]);

    qemu.printf("  Error code: 0x%X\n", error_code);
    qemu.printf("  Instruction pointer: 0x%X\n", instruction_pointer);
    qemu.printf("  Code segment: 0x%X\n", code_segment);
    qemu.printf("  CPU flags: 0x%X\n", cpu_flags);
    qemu.printf("  Stack pointer: 0x%X\n", stack_pointer);
    qemu.printf("  Stack segment: 0x%X\n", stack_segment);

    asm volatile("cli; hlt");
}