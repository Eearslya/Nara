#pragma once

#include <Nara/Defines.h>

typedef struct EventContext {
	union {
		I64 I64[2];
		U64 U64[2];
		F64 F64[2];

		I32 I32[4];
		U32 U32[4];
		F32 F32[4];
		B32 B32[4];

		I16 I16[8];
		U16 U16[8];

		I8 I8[16];
		U8 U8[16];
		B8 B8[16];
		char C[16];
	} Data;
} EventContext;

typedef enum SystemEventCode {
	Event_ApplicationQuit     = 1,
	Event_KeyPressed          = 2,
	Event_KeyReleased         = 3,
	Event_MouseButtonPressed  = 4,
	Event_MouseButtonReleased = 5,
	Event_MouseMoved          = 6,
	Event_MouseScrolled       = 7,
	Event_Resized             = 8,
} SystemEventCode;

typedef B8 (*PFN_EventHandler)(U16 code, void* sender, void* listener, EventContext context);

B8 Event_Initialize();
void Event_Shutdown();

NAPI B8 Event_Register(U16 code, void* listener, PFN_EventHandler handler);
NAPI B8 Event_Unregister(U16 code, void* listener, PFN_EventHandler handler);
NAPI B8 Event_Fire(U16 code, void* sender, EventContext context);
