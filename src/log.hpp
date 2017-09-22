#ifndef LOG_HPP
#define LOG_HPP

#include "mbed.h"

#ifdef LOG_HPP_STRIP
#define log(...) { while(0) {}; }
#else
#define log(...) _log(__VA_ARGS__)
#endif

void _log(const char* v,...);

#endif
