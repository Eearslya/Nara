#include <Nara/Core/Memory.h>
#include <Nara/Renderer/RendererBackend.h>
#include <Nara/Renderer/Vulkan/VulkanRenderer.h>

typedef struct RendererBackendState {
	B8 (*Initialize)();
	void (*Shutdown)();
	B8 (*BeginFrame)(F64);
	B8 (*EndFrame)(F64);
	void (*OnResized)(I32, I32);
} RendererBackendState;

static RendererBackendState Backend;

B8 RendererBackend_Initialize(RenderSystem system) {
	MemZero(&Backend, sizeof(Backend));

	switch (system) {
		case RenderSystem_Vulkan: {
			Backend.Initialize = Vulkan_Initialize;
			Backend.Shutdown   = Vulkan_Shutdown;
			Backend.BeginFrame = Vulkan_BeginFrame;
			Backend.EndFrame   = Vulkan_EndFrame;
			Backend.OnResized  = Vulkan_OnResized;
		} break;

		default:
			return FALSE;
	}

	if (!Backend.Initialize()) { return FALSE; }

	return TRUE;
}

void RendererBackend_Shutdown() {
	if (Backend.Shutdown) { Backend.Shutdown(); }

	MemZero(&Backend, sizeof(Backend));
}

B8 RendererBackend_BeginFrame(F64 deltaTime) {
	return Backend.BeginFrame(deltaTime);
}

B8 RendererBackend_EndFrame(F64 deltaTime) {
	return Backend.EndFrame(deltaTime);
}

void RendererBackend_OnResized(I32 w, I32 h) {
	Backend.OnResized(w, h);
}
