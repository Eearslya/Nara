#include <Nara/Core/Application.h>
#include <Nara/Core/Event.h>
#include <Nara/Core/Log.h>
#include <Nara/Core/Memory.h>
#include <Nara/Platform/Platform.h>

static ApplicationConfig Config = {};

B8 Application_Initialize(const ApplicationConfig* config) {
	if (!Platform_Initialize(config->Name, config->WindowX, config->WindowY, config->WindowW, config->WindowH)) {
		return FALSE;
	}
	if (!Memory_Initialize()) { return FALSE; }
	if (!Event_Initialize()) { return FALSE; }

	Platform_MemCopy(&Config, config, sizeof(ApplicationConfig));

	if (!Config.Initialize()) {
		LogF("[Application] Application failed to initialize!");
		return FALSE;
	}

	Config.OnResized(config->WindowW, config->WindowH);

	return TRUE;
}

void Application_Shutdown() {
	Event_Shutdown();
	Memory_Shutdown();
	Platform_Shutdown();
}

B8 Application_Run() {
	F64 lastTime = Platform_GetTime();

	while (Platform_Update()) {
		const F64 now       = Platform_GetTime();
		const F64 deltaTime = now - lastTime;
		lastTime            = now;

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
