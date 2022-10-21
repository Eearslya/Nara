#include <Nara/EntryPoint.h>
#include <Nara/Nara.h>

B8 Sandbox_Initialize() {
	void* ptr = Allocate(64, MemoryTag_Application);
	Free(ptr);

	return 1;
}

B8 Sandbox_Update(F32 deltaTime) {
	return 1;
}

B8 Sandbox_Render(F32 deltaTime) {
	return 1;
}

void Sandbox_OnResized(U32 width, U32 height) {}

B8 Application_Configure(int argc, const char** argv, ApplicationConfig* config) {
	config->Name    = "Sandbox";
	config->WindowX = -1;
	config->WindowY = -1;
	config->WindowW = 1600;
	config->WindowH = 900;

	config->Initialize = Sandbox_Initialize;
	config->Update     = Sandbox_Update;
	config->Render     = Sandbox_Render;
	config->OnResized  = Sandbox_OnResized;

	return 1;
}
