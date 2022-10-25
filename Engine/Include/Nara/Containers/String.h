#pragma once

#include <Nara/Defines.h>

NAPI char* String_Duplicate(const char* str);
NAPI B8 String_Equal(const char* a, const char* b);
NAPI U64 String_Length(const char* str);
