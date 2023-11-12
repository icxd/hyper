#include <Logging.hpp>
#include <Qemu.hpp>

namespace Hyper {

void Logging::log(LogLevel lvl, char *callee, char *fmt, ...) {
    Qemu qemu;
    qemu.puts("[");

    switch (lvl) {
    case LogLevel::Debug:
        qemu.puts("  DEBUG");
        break;
    case LogLevel::Info:
        qemu.puts("   INFO");
        break;
    case LogLevel::Warning:
        qemu.puts("WARNING");
        break;
    case LogLevel::Error:
        qemu.puts("  ERROR");
        break;
    case LogLevel::Fatal:
        qemu.puts("  FATAL");
        break;
    }

    qemu.puts("] ");
    qemu.puts(callee);
    qemu.puts(": ");

    va_list args;
    va_start(args, fmt);
    qemu.svprintf(fmt, args);
    va_end(args);

    qemu.puts("\n");
}

} // namespace Hyper