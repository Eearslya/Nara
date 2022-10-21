#include <Nara/Core/Application.h>
#include <Nara/Core/Log.h>
#include <Nara/Platform/Platform.h>

static ApplicationConfig Config = {};

B8 Application_Initialize(const ApplicationConfig* config) {
	if (!Platform_Initialize(config->Name, config->WindowX, config->WindowY, config->WindowW, config->WindowH)) {
		return 0;
	}

	Platform_MemCopy(&Config, config, sizeof(ApplicationConfig));

	if (!Config.Initialize()) {
		LogF("[Application] Application failed to initialize!");
		return 0;
	}

	Config.OnResized(config->WindowW, config->WindowH);

	return 1;
}

void Application_Shutdown() {
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
			return 0;
		}

		if (!Config.Render((F32) deltaTime)) {
			LogF("[Application] Application failed to render!");
			return 0;
		}
	}

	return 1;
}
