#pragma once

#include <Nara/Defines.h>

typedef struct Clock_T {
	F64 StartTime;
	F64 Elapsed;
} Clock;

NAPI void Clock_Start(Clock* c);
NAPI void Clock_Update(Clock* c);
NAPI void Clock_Stop(Clock* c);
