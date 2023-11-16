#include <Common.hpp>
#include <Isr.hpp>
#include <Ports.hpp>
#include <stdio.h>

namespace Hyper {

void Isr::isrInstall(Idt *idt) {
    idt->setDescriptor(0, (void *)isr_stub_0, 0x8E);
    idt->setDescriptor(1, (void *)isr_stub_1, 0x8E);
    idt->setDescriptor(2, (void *)isr_stub_2, 0x8E);
    idt->setDescriptor(3, (void *)isr_stub_3, 0x8E);
    idt->setDescriptor(4, (void *)isr_stub_4, 0x8E);
    idt->setDescriptor(5, (void *)isr_stub_5, 0x8E);
    idt->setDescriptor(6, (void *)isr_stub_6, 0x8E);
    idt->setDescriptor(7, (void *)isr_stub_7, 0x8E);
    idt->setDescriptor(8, (void *)isr_stub_8, 0x8E);
    idt->setDescriptor(9, (void *)isr_stub_9, 0x8E);
    idt->setDescriptor(10, (void *)isr_stub_10, 0x8E);
    idt->setDescriptor(11, (void *)isr_stub_11, 0x8E);
    idt->setDescriptor(12, (void *)isr_stub_12, 0x8E);
    idt->setDescriptor(13, (void *)isr_stub_13, 0x8E);
    idt->setDescriptor(14, (void *)isr_stub_14, 0x8E);
    idt->setDescriptor(15, (void *)isr_stub_15, 0x8E);
    idt->setDescriptor(16, (void *)isr_stub_16, 0x8E);
    idt->setDescriptor(17, (void *)isr_stub_17, 0x8E);
    idt->setDescriptor(18, (void *)isr_stub_18, 0x8E);
    idt->setDescriptor(19, (void *)isr_stub_19, 0x8E);
    idt->setDescriptor(20, (void *)isr_stub_20, 0x8E);
    idt->setDescriptor(21, (void *)isr_stub_21, 0x8E);
    idt->setDescriptor(22, (void *)isr_stub_22, 0x8E);
    idt->setDescriptor(23, (void *)isr_stub_23, 0x8E);
    idt->setDescriptor(24, (void *)isr_stub_24, 0x8E);
    idt->setDescriptor(25, (void *)isr_stub_25, 0x8E);
    idt->setDescriptor(26, (void *)isr_stub_26, 0x8E);
    idt->setDescriptor(27, (void *)isr_stub_27, 0x8E);
    idt->setDescriptor(28, (void *)isr_stub_28, 0x8E);
    idt->setDescriptor(29, (void *)isr_stub_29, 0x8E);
    idt->setDescriptor(30, (void *)isr_stub_30, 0x8E);
    idt->setDescriptor(31, (void *)isr_stub_31, 0x8E);

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