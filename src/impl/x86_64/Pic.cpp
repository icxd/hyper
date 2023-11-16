#include <Pic.hpp>
#include <Ports.hpp>

namespace Hyper {

PIC::PIC() {}

void PIC::remap(uint8_t offset1, uint8_t offset2) {
    uint8_t a1, a2;

    a1 = portByteIn(PIC1_DATA); // save masks
    a2 = portByteIn(PIC2_DATA);

    portByteOut(
        PIC1_COMMAND,
        ICW1_INIT |
            ICW1_ICW4); // starts the initialization sequence (in cascade mode)
    portIoWait();
    portByteOut(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    portIoWait();
    portByteOut(PIC1_DATA, offset1); // ICW2: Master PIC vector offset
    portIoWait();
    portByteOut(PIC2_DATA, offset2); // ICW2: Slave PIC vector offset
    portIoWait();
    portByteOut(PIC1_DATA, 4); // ICW3: tell Master PIC that there is a slave
                               // PIC at IRQ2 (0000 0100)
    portIoWait();
    portByteOut(PIC2_DATA,
                2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    portIoWait();

    portByteOut(PIC1_DATA, ICW4_8086);
    portIoWait();
    portByteOut(PIC2_DATA, ICW4_8086);
    portIoWait();

    portByteOut(PIC1_DATA, a1); // restore saved masks.
    portByteOut(PIC2_DATA, a2);
}

void PIC::mask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = portByteIn(port) | (1 << irq);
    portByteOut(port, value);
}

void PIC::unmask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = portByteIn(port) & ~(1 << irq);
    portByteOut(port, value);
}

void PIC::eoi(uint8_t irq) {
    if (irq >= 8) {
        portByteOut(PIC2_COMMAND, PIC_EOI);
    }
    portByteOut(PIC1_COMMAND, PIC_EOI);
}

} // namespace Hyper