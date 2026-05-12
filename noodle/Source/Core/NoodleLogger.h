#pragma once

typedef void (*LogCallback)(const char*);

void LogRegisterCallback(LogCallback cb);
void LogWrite(const char* fmt, ...);

#ifdef _DEBUG
#define N_LOG(...) LogWrite(__VA_ARGS__)
#else
#define N_LOG(...) ((void)0)
#endif