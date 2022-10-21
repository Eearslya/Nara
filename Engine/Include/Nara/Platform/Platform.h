#pragma once

#include <Nara/Defines.h>

NAPI B8 Platform_Initialize(const char* appName, I32 startX, I32 startY, U32 width, U32 height);
NAPI void Platform_Shutdown();
NAPI B8 Platform_Update();

void* Platform_Allocate(U64 size, U64 alignment);
void Platform_Free(void* ptr);
void Platform_MemCopy(void* dst, const void* src, U64 size);
void Platform_MemSet(void* dst, U8 value, U64 size);
void Platform_MemZero(void* dst, U64 size);

F64 Platform_GetTime();
