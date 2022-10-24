#include <Nara/Core/Application.h>
#include <Nara/Core/Clock.h>
#include <Nara/Core/Event.h>
#include <Nara/Core/Input.h>
#include <Nara/Core/Log.h>
#include <Nara/Core/Memory.h>
#include <Nara/Platform/Platform.h>

typedef struct ApplicationState {
	B8 Running;
	Clock Clock;
	U32 TargetFPS;
} ApplicationState;

static ApplicationConfig Config     = {};
static ApplicationState Application = {};

B8 Application_OnEvent(U16 code, void* sender, void* listener, EventContext context);

B8 Application_Initialize(const ApplicationConfig* config) {
	if (!Platform_Initialize(config->Name, config->WindowX, config->WindowY, config->WindowW, config->WindowH)) {
		return FALSE;
	}
	if (!Memory_Initialize()) { return FALSE; }
	if (!Event_Initialize()) { return FALSE; }
	if (!Input_Initialize()) { return FALSE; }

	MemCopy(&Config, config, sizeof(ApplicationConfig));
	MemZero(&Application, sizeof(Application));

	if (!Config.Initialize()) {
		LogF("[Application] Application failed to initialize!");
		return FALSE;
	}

	Application.TargetFPS = 60;
	Config.OnResized(config->WindowW, config->WindowH);

	Event_Register(Event_ApplicationQuit, &Application, Application_OnEvent);
	Event_Register(Event_KeyPressed, &Application, Application_OnEvent);
	Event_Register(Event_KeyReleased, &Application, Application_OnEvent);

	return TRUE;
}

void Application_Shutdown() {
	Event_Unregister(Event_ApplicationQuit, &Application, Application_OnEvent);
	Event_Unregister(Event_KeyPressed, &Application, Application_OnEvent);
	Event_Unregister(Event_KeyReleased, &Application, Application_OnEvent);

	Input_Shutdown();
	Event_Shutdown();
	Memory_Shutdown();
	Platform_Shutdown();
}

B8 Application_Run() {
	Application.Running = TRUE;
	Clock_Start(&Application.Clock);
	Clock_Update(&Application.Clock);

	Clock frameClock;
	while (Application.Running) {
		const F64 lastTime = Application.Clock.Elapsed;
		Clock_Update(&Application.Clock);
		const F64 now       = Application.Clock.Elapsed;
		const F64 deltaTime = now - lastTime;

		Clock_Start(&frameClock);

		if (!Platform_Update()) {
			Application.Running = FALSE;
			break;
		}

		Input_Update(deltaTime);

		if (!Config.Update((F32) deltaTime)) {
			LogF("[Application] Application failed to update!");
			break;
		}

		if (!Config.Render((F32) deltaTime)) {
			LogF("[Application] Application failed to render!");
			break;
		}

		Clock_Update(&frameClock);
		if (Application.TargetFPS > 0) {
			const F64 targetSeconds = 1.0 / (F64) Application.TargetFPS;
			const F64 remainingTime = targetSeconds - frameClock.Elapsed;
			if (remainingTime > 0.0) {
				const U64 remainingMs = remainingTime * 1000.0;
				if (remainingMs > 0.0) { Platform_Sleep(remainingMs - 1); }
			}
		}
	}

	Clock_Stop(&Application.Clock);

	// If we ended the application loop in error, Running will not be reset to FALSE.
	// This way, if Running is TRUE when we return, we know there was an error.
	const B8 applicationErrored = Application.Running == TRUE;
	Application.Running         = FALSE;

	return !applicationErrored;
}

B8 Application_OnEvent(U16 code, void* sender, void* listener, EventContext context) {
	switch (code) {
		case Event_ApplicationQuit: {
			Application.Running = FALSE;

			return TRUE;
		} break;

		case Event_KeyPressed: {
			LogI("[Application] KeyPressed: %u", context.Data.U8[0]);
		} break;

		case Event_KeyReleased: {
			LogI("[Application] KeyReleased: %u", context.Data.U8[0]);
		} break;
	}

	return FALSE;
}
