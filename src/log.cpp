#include "log.hpp"

namespace {
    Serial pc(USBTX, USBRX);
};

void _log(const char* format,...) {
    std::va_list arg;
    va_start(arg, format);
    pc.vprintf(format, arg);
    va_end(arg);
    return;
}
