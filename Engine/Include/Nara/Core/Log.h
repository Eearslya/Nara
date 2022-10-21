#pragma once

#include <Nara/Defines.h>

#define NARA_LOG_WARN  1
#define NARA_LOG_INFO  1
#define NARA_LOG_DEBUG 1
#define NARA_LOG_TRACE 1

typedef enum LogLevel {
	LogLevel_Fatal = 0,
	LogLevel_Error = 1,
	LogLevel_Warn  = 2,
	LogLevel_Info  = 3,
	LogLevel_Debug = 4,
	LogLevel_Trace = 5
} LogLevel;

NAPI void Log_Output(LogLevel level, const char* fmt, ...);

#define LogF(fmt, ...) Log_Output(LogLevel_Fatal, fmt, ##__VA_ARGS__);
#define LogE(fmt, ...) Log_Output(LogLevel_Error, fmt, ##__VA_ARGS__);

#if NARA_LOG_WARN
#	define LogW(fmt, ...) Log_Output(LogLevel_Warn, fmt, ##__VA_ARGS__);
#else
#	define LogW(fmt, ...)
#endif
#if NARA_LOG_INFO
#	define LogI(fmt, ...) Log_Output(LogLevel_Info, fmt, ##__VA_ARGS__);
#else
#	define LogI(fmt, ...)
#endif
#if NARA_LOG_DEBUG
#	define LogD(fmt, ...) Log_Output(LogLevel_Debug, fmt, ##__VA_ARGS__);
#else
#	define LogD(fmt, ...)
#endif
#if NARA_LOG_TRACE
#	define LogT(fmt, ...) Log_Output(LogLevel_Trace, fmt, ##__VA_ARGS__);
#else
#	define LogT(fmt, ...)
#endif
