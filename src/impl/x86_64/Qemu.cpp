#include <Ports.hpp>
#include <Qemu.hpp>
#include <Serial.hpp>

#include <stdarg.h>

namespace Hyper {

void Qemu::putchar(char ch) {
    while ((portByteIn(qemu_log_serial_port + 5) & 0x20) == 0)
        ;
    portByteOut(qemu_log_serial_port, ch);
}

void Qemu::puts(char *string) {
    for (int i = 0; string[i] != '\0'; i++)
        this->putchar(string[i]);
}

void Qemu::svprintf(const char *format, va_list args) {
    for (int i = 0; format[i] != '\0'; i++) {
        // %[flags][width][.precision][length]specifier
        if (format[i] == '%') {
            i++;

            enum class Flags { None = 0, Minus, Plus, Space, Hash, Zero };
            enum class Width { None = 0, Number, Asterisk };
            enum class Prescision { None = 0, Number, Asterisk };
            enum class Length { None = 0, hh, h, l, ll, j, z, t, L };

            Flags flags = Flags::None;
            Width width = Width::None;
            Prescision precision = Prescision::None;
            Length length = Length::None;

            switch (format[i]) {
            case '-': flags = Flags::Minus; i++; break;
            case '+': flags = Flags::Plus;  i++; break;
            case ' ': flags = Flags::Space; i++; break;
            case '#': flags = Flags::Hash;  i++; break;
            case '0': flags = Flags::Zero;  i++; break;
            default: break;
            }

            if (format[i] == '*') {
                width = Width::Asterisk;
                i++;
            } else if (format[i] >= '0' && format[i] <= '9')
                width = Width::Number;

            if (format[i] == '.') {
                i++;
                if (format[i] == '*') {
                    precision = Prescision::Asterisk;
                    i++;
                } else if (format[i] >= '0' && format[i] <= '9')
                    precision = Prescision::Number;
            }

            switch (format[i]) {
            case 'h':
                if (format[i + 1] == 'h') {
                    length = Length::hh;
                    i++;
                } else
                    length = Length::h;
                i++;
                break;
            case 'l':
                if (format[i + 1] == 'l') {
                    length = Length::ll;
                    i++;
                } else
                    length = Length::l;
                i++;
                break;
            case 'j': length = Length::j; i++; break;
            case 'z': length = Length::z; i++; break;
            case 't': length = Length::t; i++; break;
            case 'L': length = Length::L; i++; break;
            default: break;
            }

            switch (format[i]) {
            case 'd':
            case 'i': {
                int64_t value = 0;
                if (length == Length::hh)       value = va_arg(args, int);
                else if (length == Length::h)   value = va_arg(args, int);
                else if (length == Length::l)   value = va_arg(args, long);
                else if (length == Length::ll)  value = va_arg(args, long long);
                else                            value = va_arg(args, int);

                if (value < 0) {
                    this->putchar('-');
                    value = -value;
                }

                char buffer[32];
                int index = 0;
                while (value != 0) {
                    buffer[index++] = '0' + (value % 10);
                    value /= 10;
                }

                for (int i = index - 1; i >= 0; i--)
                    this->putchar(buffer[i]);
                break;
            }
            case 'u': {
                uint64_t value = 0;
                if (length == Length::hh)       value = va_arg(args, unsigned int);
                else if (length == Length::h)   value = va_arg(args, unsigned int);
                else if (length == Length::l)   value = va_arg(args, unsigned long);
                else if (length == Length::ll)  value = va_arg(args, unsigned long long);
                else                            value = va_arg(args, unsigned int);

                char buffer[32];
                int index = 0;
                while (value != 0) {
                    buffer[index++] = '0' + (value % 10);
                    value /= 10;
                }

                for (int i = index - 1; i >= 0; i--)
                    this->putchar(buffer[i]);
                break;
            }
            case 'x':
            case 'X': {
                bool upper = format[i] == 'X';
                uint64_t value = 0;
                if (length == Length::hh)       value = va_arg(args, unsigned int);
                else if (length == Length::h)   value = va_arg(args, unsigned int);
                else if (length == Length::l)   value = va_arg(args, unsigned long);
                else if (length == Length::ll)  value = va_arg(args, unsigned long long);
                else                            value = va_arg(args, unsigned int);

                char buffer[32];
                int index = 0;
                while (value != 0) {
                    uint8_t digit = value % 16;
                    if (digit < 10)
                        buffer[index++] = '0' + digit;
                    else
                        buffer[index++] = (upper ? 'A' : 'a') + (digit - 10);
                    value /= 16;
                }

                for (int i = index - 1; i >= 0; i--)
                    this->putchar(buffer[i]);
                break;
            }
            case 'o': {
                uint64_t value = 0;
                if (length == Length::hh)       value = va_arg(args, unsigned int);
                else if (length == Length::h)   value = va_arg(args, unsigned int);
                else if (length == Length::l)   value = va_arg(args, unsigned long);
                else if (length == Length::ll)  value = va_arg(args, unsigned long long);
                else                            value = va_arg(args, unsigned int);

                char buffer[32];
                int index = 0;
                while (value != 0) {
                    buffer[index++] = '0' + (value % 8);
                    value /= 8;
                }

                for (int i = index - 1; i >= 0; i--)
                    this->putchar(buffer[i]);
                break;
            }
            case 'f': { // Floating point number
                double value = va_arg(args, double);
                if (value < 0) {
                    this->putchar('-');
                    value = -value;
                }

                int64_t integer = (int64_t)value;
                double fraction = value - integer;

                char buffer[32];
                int index = 0;
                while (integer != 0) {
                    buffer[index++] = '0' + (integer % 10);
                    integer /= 10;
                }

                for (int i = index - 1; i >= 0; i--)
                    this->putchar(buffer[i]);
                this->putchar('.');
                for (int i = 0; i < 6; i++) {
                    fraction *= 10;
                    this->putchar('0' + (int)fraction);
                    fraction -= (int)fraction;
                }

                break;
            }
            case 'c': {
                char value = va_arg(args, int);
                this->putchar(value);
                break;
            }
            case 's': {
                char *value = va_arg(args, char *);
                this->puts(value);
                break;
            }
            case 'p': {
                uint64_t value = va_arg(args, uint64_t);
                this->putchar('0');
                this->putchar('x');

                char buffer[32];
                int index = 0;
                while (value != 0) {
                    uint8_t digit = value % 16;
                    if (digit < 10)
                        buffer[index++] = '0' + digit;
                    else
                        buffer[index++] = 'a' + (digit - 10);
                    value /= 16;
                }

                for (int i = index - 1; i >= 0; i--)
                    this->putchar(buffer[i]);
                break;
            }
            case '%': {
                this->putchar('%');
                break;
            }
            default: break;
            }
        } else {
            this->putchar(format[i]);
        }
    }
}

void Qemu::printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    this->svprintf(format, args);
    va_end(args);
}

} // namespace Hyper