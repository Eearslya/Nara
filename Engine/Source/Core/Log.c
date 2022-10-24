#include <Nara/Core/Log.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static const char* LogLevel_Names[] = {"Fatal", "Error", "Warn", "Info", "Debug", "Trace"};

static LogLevel Log_CurrentLevel        = LogLevel_Trace;
static _Thread_local char* Log_Buffer   = NULL;
static _Thread_local U64 Log_BufferSize = 0;

void Log_Output(LogLevel level, const char* fmt, ...) {
	if (level > LogLevel_Error && level > Log_CurrentLevel) { return; }

	__builtin_va_list args;
	va_start(args, fmt);

	const I32 tagLen = strlen(LogLevel_Names[level]) + 3;
	const I32 msgLen = vsnprintf(NULL, 0, fmt, args);
	const I32 logLen = tagLen + msgLen + 1;
	if (logLen > Log_BufferSize) {
		Log_Buffer     = realloc(Log_Buffer, logLen);
		Log_BufferSize = logLen;
	}

	char* bufCursor = Log_Buffer;
	bufCursor += snprintf(bufCursor, Log_BufferSize - (bufCursor - Log_Buffer), "[%s] ", LogLevel_Names[level]);
	bufCursor += vsnprintf(bufCursor, Log_BufferSize - (bufCursor - Log_Buffer), fmt, args);

	if (level > LogLevel_Error) {
		fprintf(stdout, "%s\n", Log_Buffer);
		fflush(stdout);
	} else {
		fprintf(stderr, "%s\n", Log_Buffer);
		fflush(stderr);
	}
}
