#pragma once

#include <Serial.hpp>

#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace Hyper {

constexpr uint16_t qemu_log_serial_port = 0x3F8;

class Qemu {
  public:
    enum class Color {
        Black = 30,
        Red,
        Green,
        Yellow,
        Blue,
        Pink,
        Cyan,
        White,

        LightBlack = 90,
        LightRed,
        LightGreen,
        LightYellow,
        LightBlue,
        LightPink,
        LightCyan,
        LightWhite,
    };

  public:
    Qemu() : m_port(initSerial(qemu_log_serial_port)) {}
    ~Qemu() = default;

    void putchar(char ch);
    void puts(char *string);

    void svprintf(const char *format, va_list args);
    void printf(const char *format, ...);

    uint32_t port() const { return this->m_port; }

  private:
    uint32_t m_port;
};

} // namespace Hyper

#ifdef __cplusplus
}
#endif