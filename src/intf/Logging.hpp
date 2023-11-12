#pragma once

#ifdef __cplusplus
extern "C" {
#endif

namespace Hyper {

class Logging {
  public:
    enum class LogLevel { Debug, Info, Warning, Error, Fatal };

  public:
    static void log(LogLevel, char *, char *, ...);

    static void debug(char *callee, char *fmt, ...) {
        log(LogLevel::Debug, callee, fmt);
    }

    static void info(char *callee, char *fmt, ...) {
        log(LogLevel::Info, callee, fmt);
    }

    static void warning(char *callee, char *fmt, ...) {
        log(LogLevel::Warning, callee, fmt);
    }

    static void error(char *callee, char *fmt, ...) {
        log(LogLevel::Error, callee, fmt);
    }

    static void fatal(char *callee, char *fmt, ...) {
        log(LogLevel::Fatal, callee, fmt);
    }
};

} // namespace Hyper

#ifdef __cplusplus
}
#endif