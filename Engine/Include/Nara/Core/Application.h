#pragma once

#include <Nara/Defines.h>

typedef struct ApplicationConfig {
	const char* Name;
	I32 WindowX;
	I32 WindowY;
	U32 WindowW;
	U32 WindowH;

	B8 (*Initialize)();
	B8 (*Update)(F32);
	B8 (*Render)(F32);

	void (*OnResized)(U32, U32);
} ApplicationConfig;

extern B8 Application_Configure(int, const char**, ApplicationConfig*);

NAPI B8 Application_Initialize(const ApplicationConfig* config);
NAPI void Application_Shutdown();

NAPI B8 Application_Run();
