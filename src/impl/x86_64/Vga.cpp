#include <Vga.hpp>

#include <stdarg.h>
#include <stdint.h>

namespace Hyper {

void Vga::clear() {
    for (uint64_t i = 0; i < NUM_ROWS; i++) {
        this->clearRow(i);
    }
}

void Vga::putchar(char ch) {
    if (ch == '\n') {
        this->putNewline();
        return;
    }

    if (this->m_col >= NUM_COLS) {
        this->putNewline();
    }

    this->m_buffer[this->m_col + NUM_COLS * this->m_row] = (Vga::Char){
        .character = (uint8_t)ch,
        .color = this->m_color,
    };

    this->m_col++;
}

void Vga::puts(char *string) {
    for (uint64_t i = 0; string[i] != '\0'; i++) {
        this->putchar(string[i]);
    }
}

void Vga::printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (uint64_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            switch (format[++i]) {
            case 'c': {
                char ch = (char)va_arg(args, int);
                this->putchar(ch);
                break;
            }
            case 's': {
                char *string = va_arg(args, char *);
                this->puts(string);
                break;
            }
            case 'd': {
                int64_t num = va_arg(args, int64_t);
                if (num < 0) {
                    this->putchar('-');
                    num = -num;
                }
                char str[21];
                uint64_t len = 0;
                do {
                    str[len++] = (char)(num % 10 + '0');
                    num /= 10;
                } while (num > 0);
                for (uint64_t i = 0; i < len / 2; i++) {
                    char tmp = str[i];
                    str[i] = str[len - i - 1];
                    str[len - i - 1] = tmp;
                }
                str[len] = '\0';
                this->puts(str);
                break;
            }
            case 'x': {
                uint64_t num = va_arg(args, uint64_t);
                char str[17];
                uint64_t len = 0;
                do {
                    uint64_t digit = num % 16;
                    if (digit < 10) {
                        str[len++] = (char)(digit + '0');
                    } else {
                        str[len++] = (char)(digit - 10 + 'a');
                    }
                    num /= 16;
                } while (num > 0);
                for (uint64_t i = 0; i < len / 2; i++) {
                    char tmp = str[i];
                    str[i] = str[len - i - 1];
                    str[len - i - 1] = tmp;
                }
                str[len] = '\0';
                this->puts(str);
                break;
            }
            case '%': {
                this->putchar('%');
                break;
            }
            }
        } else {
            this->putchar(format[i]);
        }
    }

    va_end(args);
}

void Vga::setColor(Vga::Color fg, Vga::Color bg) {
    this->m_color = static_cast<uint8_t>(fg) + (static_cast<uint8_t>(bg) << 4);
}

void Vga::clearRow(uint64_t row) {
    Vga::Char empty = (Vga::Char){
        .character = ' ',
        .color = this->m_color,
    };

    for (uint64_t i = 0; i < NUM_COLS; i++) {
        this->m_buffer[row * NUM_COLS + i] = empty;
    }
}

void Vga::putNewline() {
    this->m_col = 0;

    if (this->m_row < NUM_ROWS - 1) {
        this->m_row++;
        return;
    }

    for (uint64_t row = 1; row < NUM_ROWS; row++) {
        for (uint64_t col = 0; col < NUM_COLS; col++) {
            Vga::Char character = this->m_buffer[row * NUM_COLS + col];
            this->m_buffer[col + NUM_COLS * (row - 1)] = character;
        }
    }

    this->clearRow(NUM_ROWS - 1);
}

} // namespace Hyper