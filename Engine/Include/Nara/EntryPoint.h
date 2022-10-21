#pragma once

#include <Nara/Core/Application.h>
#include <Nara/Core/Assert.h>
#include <Nara/Platform/Platform.h>

int main(int argc, const char** argv) {
	ApplicationConfig config = {};
	if (!Application_Configure(argc, argv, &config)) { return 1; }

	AssertMsg(config.Initialize != NULL && config.Update != NULL && config.Render != NULL && config.OnResized != NULL,
	          "ApplicationConfig must set all callback functions!");

	if (!Application_Initialize(&config)) { return 1; }

	if (!Application_Run()) { return 1; }

	Application_Shutdown();

	return 0;
}
