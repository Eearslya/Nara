#include <Nara/Containers/DynArray.h>
#include <Nara/Core/Event.h>
#include <Nara/Core/Memory.h>

typedef struct RegisteredEvent {
	void* Listener;
	PFN_EventHandler Handler;
} RegisteredEvent;

typedef struct EventCodeEntry {
	RegisteredEvent* Events;  // DynArray
} EventCodeEntry;

#define EventCodeCount 16384
STATIC_ASSERT(EventCodeCount < (U16) -1, "Maximum event code count exceeds storage capabilities of U16!");

static struct EventState { EventCodeEntry Events[EventCodeCount]; } Event;

B8 Event_Initialize() {
	MemZero(&Event, sizeof(Event));

	return TRUE;
}

void Event_Shutdown() {
	for (U16 i = 0; i < EventCodeCount; ++i) { DynArray_Destroy(Event.Events[i].Events); }
}

B8 Event_Register(U16 code, void* listener, PFN_EventHandler handler) {
	if (Event.Events[code].Events == NULL) { Event.Events[code].Events = DynArray_Create(RegisteredEvent); }

	U64 registeredCount = DynArray_Size(Event.Events[code].Events);
	for (U64 i = 0; i < registeredCount; ++i) {
		if (Event.Events[code].Events[i].Listener == listener) { return FALSE; }
	}

	RegisteredEvent event = {.Listener = listener, .Handler = handler};
	DynArray_PushValue(Event.Events[code].Events, event);

	return TRUE;
}

B8 Event_Unregister(U16 code, void* listener, PFN_EventHandler handler) {
	if (Event.Events[code].Events == NULL) { return FALSE; }

	U64 registeredCount = DynArray_Size(Event.Events[code].Events);
	for (U64 i = 0; i < registeredCount; ++i) {
		if (Event.Events[code].Events[i].Listener == listener && Event.Events[code].Events[i].Handler == handler) {
			DynArray_Erase(Event.Events[code].Events, i);
			return TRUE;
		}
	}

	return FALSE;
}

B8 Event_Fire(U16 code, void* sender, EventContext context) {
	if (Event.Events[code].Events == NULL) { return FALSE; }

	U64 registeredCount = DynArray_Size(Event.Events[code].Events);
	for (U64 i = 0; i < registeredCount; ++i) {
		if (Event.Events[code].Events[i].Handler(code, sender, Event.Events[code].Events[i].Listener, context)) {
			return TRUE;
		}
	}

	return FALSE;
}
