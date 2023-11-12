#include <Idt.hpp>

#define GDT_OFFSET_KERNEL_CODE 0x08 // most likely invalid, double check later
#define IDT_MAX_DESCRIPTORS 256

extern void *isr_stub_table[];

namespace Hyper {

Idt::Idt() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(IdtEntry) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        setDescriptor(vector, isr_stub_table[vector], 0x8e);
        // vectors[vector] = true;
    }

    __asm__ volatile("lidt %0" : : "m"(idtr)); // load new idt
    __asm__ volatile("sti");                   // set the interrupt flag
}

void Idt::setDescriptor(uint8_t vector, void *isr, uint8_t flags) {
    IdtEntry *descriptor = &idt[vector];

    descriptor->isr_low = (uint64_t)isr & 0xffff;
    descriptor->kernel_cs = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid = ((uint64_t)isr >> 16) & 0xffff;
    descriptor->isr_high = ((uint64_t)isr >> 32) & 0xffffffff;
    descriptor->reserved = 0;
}

} // namespace Hyper

void exception_handler() { __asm__ volatile("cli; hlt"); }