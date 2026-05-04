#include "NoodleLogger.h"
#include <stdio.h>
#include <stdarg.h>

static LogCallback sCallback = nullptr;

void LogRegisterCallback(LogCallback cb)
{
    sCallback = cb;
}

void LogWrite(const char* fmt, ...)
{
    char buffer[2048];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (sCallback)
    {
        sCallback(buffer);
    }
}