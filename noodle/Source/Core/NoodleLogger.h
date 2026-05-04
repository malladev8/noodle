#pragma once

typedef void (*LogCallback)(const char*);

void LogRegisterCallback(LogCallback cb);
void LogWrite(const char* fmt, ...);

#ifdef _DEBUG
#define LOG(...) LogWrite(__VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif