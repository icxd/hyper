#include <Common.hpp>
#include <Isr.hpp>
#include <Ports.hpp>
#include <stdio.h>

namespace Hyper {

void Isr::isrInstall(Idt *idt) {
    printf("sizeof(ist_stub_table) = %llu\n",
           sizeof(isr_stub_table) / sizeof(isr_stub_table[0]));
    for (uint8_t vector = 0; vector < 256; vector++)
        idt->setDescriptor(vector, isr_stub_table[vector], 0x8E);

    portByteOut(0x20, 0x11);
    portByteOut(0xA0, 0x11);
    portByteOut(0x21, 0x20);
    portByteOut(0xA1, 0x28);
    portByteOut(0x21, 0x04);
    portByteOut(0xA1, 0x02);
    portByteOut(0x21, 0x01);
    portByteOut(0xA1, 0x01);
    portByteOut(0x21, 0x0);
    portByteOut(0xA1, 0x0);

    idt->install();
}

void Isr::irqInstall() { asm volatile("sti"); }

void Isr::isrHandler(Registers *registers) {
    if (registers->int_no >= 40) portByteOut(0xA0, 0x20);
    portByteOut(0x20, 0x20);

    if (handlers[registers->int_no] != 0) {
        IsrHandler handler = handlers[registers->int_no];
        handler(registers);
    } else {
        PANIC("Unhandled interrupt: %d", registers->int_no);
    }
}

void Isr::registerHandler(uint8_t n, IsrHandler handler) {
    handlers[n] = handler;
}

} // namespace Hyper