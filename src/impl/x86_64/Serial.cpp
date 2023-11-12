#include <Ports.hpp>
#include <Serial.hpp>

namespace Hyper {

int initSerial(int port) {
    portByteOut(port + 1, 0x00); // Disable all interrupts
    portByteOut(port + 3, 0x80); // Enable DLAB (set baud rate divisor)
    portByteOut(port + 0, 0x0C); // Set divisor to 3 (lo byte) 38400 baud
    portByteOut(port + 1, 0x00); //                  (hi byte)
    portByteOut(port + 3, 0x03); // 8 bits, no parity, one stop bit
    portByteOut(port + 2,
                0xC7); // Enable FIFO, clear them, with 14-byte threshold
    portByteOut(port + 4, 0x0B); // IRQs enabled, RTS/DSR set
    // portByteOut(port + 4, 0x1E); // Set in loopback mode, test the serial
    // chip
    portByteOut(port + 0, 0xAE); // Test serial chip (send byte 0xAE and check
                                 // if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (portByteIn(port + 0) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    portByteOut(port + 4, 0x0F);
    return 0;
}

} // namespace Hyper