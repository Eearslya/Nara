#include <Nara/Core/Application.h>
#include <Nara/Core/Event.h>
#include <Nara/Core/Input.h>
#include <Nara/Core/Log.h>
#include <Nara/Core/Memory.h>
#include <Nara/Platform/Platform.h>

typedef struct ApplicationState {
	B8 Running;
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
	F64 lastTime = Platform_GetTime();

	Application.Running = TRUE;
	while (Application.Running) {
		if (!Platform_Update()) {
			Application.Running = FALSE;
			break;
		}

		const F64 now       = Platform_GetTime();
		const F64 deltaTime = now - lastTime;
		lastTime            = now;

		Input_Update(deltaTime);

		if (!Config.Update((F32) deltaTime)) {
			LogF("[Application] Application failed to update!");
			return FALSE;
		}

		if (!Config.Render((F32) deltaTime)) {
			LogF("[Application] Application failed to render!");
			return FALSE;
		}
	}

	return TRUE;
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
