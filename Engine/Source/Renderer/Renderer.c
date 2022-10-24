#include <Nara/Core/Assert.h>
#include <Nara/Core/Event.h>
#include <Nara/Renderer/Renderer.h>
#include <Nara/Renderer/RendererBackend.h>

B8 Renderer_OnResized(U16 code, void* sender, void* listener, EventContext context);

B8 Renderer_Initialize() {
	if (!RendererBackend_Initialize(RenderSystem_Vulkan)) { return FALSE; }

	Event_Register(Event_Resized, NULL, Renderer_OnResized);

	return TRUE;
}

void Renderer_Shutdown() {
	Event_Unregister(Event_Resized, NULL, Renderer_OnResized);

	RendererBackend_Shutdown();
}

B8 Renderer_DrawFrame(const RenderPacket* packet) {
	Assert(packet);

	if (RendererBackend_BeginFrame(packet->DeltaTime)) {
		if (!RendererBackend_EndFrame(packet->DeltaTime)) { return FALSE; }
	}

	return TRUE;
}

B8 Renderer_OnResized(U16 code, void* sender, void* listener, EventContext context) {
	return FALSE;
}
