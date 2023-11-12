#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace Hyper {

constexpr uint64_t NUM_COLS = 80;
constexpr uint64_t NUM_ROWS = 25;

class Vga {
  public:
    enum class Color : uint8_t {
        Black = 0,
        Blue = 1,
        Green = 2,
        Cyan = 3,
        Red = 4,
        Magenta = 5,
        Brown = 6,
        LightGrey = 7,
        DarkGrey = 8,
        LightBlue = 9,
        LightGreen = 10,
        LightCyan = 11,
        LightRed = 12,
        Pink = 13,
        Yellow = 14,
        White = 15
    };

    struct Char {
        uint8_t character, color;
    };

  public:
    void clear();
    void putchar(char ch);
    void puts(char *string);
    void printf(const char *format, ...);
    void setColor(Vga::Color fg, Vga::Color bg);

    uint64_t row() const { return this->m_row; }
    uint64_t column() const { return this->m_col; }
    uint8_t color() const { return this->m_color; }
    Vga::Char *buffer() const { return this->m_buffer; }

  private:
    void clearRow(uint64_t row);
    void putNewline();

  private:
    uint64_t m_row = 0, m_col = 0;
    uint8_t m_color = static_cast<uint8_t>(Vga::Color::White) |
                      static_cast<uint8_t>(Vga::Color::Black) << 4;
    Vga::Char *m_buffer = (Vga::Char *)0xb8000;
};

} // namespace Hyper

#ifdef __cplusplus
}
#endif