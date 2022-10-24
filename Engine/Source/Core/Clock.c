#include <Nara/Core/Assert.h>
#include <Nara/Core/Clock.h>
#include <Nara/Platform/Platform.h>

void Clock_Start(Clock* c) {
	Assert(c);

	c->StartTime = Platform_GetTime();
	c->Elapsed   = 0.0;
}

void Clock_Update(Clock* c) {
	Assert(c);

	if (c->StartTime != 0.0) { c->Elapsed = Platform_GetTime() - c->StartTime; }
}

void Clock_Stop(Clock* c) {
	Assert(c);

	c->StartTime = 0.0;
}
