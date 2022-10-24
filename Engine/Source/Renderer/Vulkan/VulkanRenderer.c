#include <Nara/Core/Memory.h>
#include <Nara/Renderer/Vulkan/VulkanRenderer.h>
#include <Nara/Renderer/Vulkan/VulkanTypes.h>

typedef struct VulkanContext {
	VkInstance Instance;
	VkAllocationCallbacks Allocator;
} VulkanContext;

static VulkanContext* Vulkan = NULL;

void* Vulkan_Allocate(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope scope) {
	return AllocateAligned(size, alignment, MemoryTag_Renderer);
}

void* Vulkan_Reallocate(
	void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope scope) {
	return ReallocateAligned(pOriginal, size, alignment, MemoryTag_Renderer);
}

void Vulkan_Free(void* pUserData, void* pMemory) {
	Free(pMemory);
}

B8 Vulkan_Initialize() {
	Vulkan = Allocate(sizeof(*Vulkan), MemoryTag_Renderer);
	MemZero(Vulkan, sizeof(*Vulkan));

	Vulkan->Allocator.pfnAllocation   = Vulkan_Allocate;
	Vulkan->Allocator.pfnReallocation = Vulkan_Reallocate;
	Vulkan->Allocator.pfnFree         = Vulkan_Free;

	const VkInstanceCreateInfo instanceCI = {.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	                                         .pNext                   = NULL,
	                                         .flags                   = 0,
	                                         .pApplicationInfo        = NULL,
	                                         .enabledLayerCount       = 0,
	                                         .ppEnabledLayerNames     = NULL,
	                                         .enabledExtensionCount   = 0,
	                                         .ppEnabledExtensionNames = NULL};
	const VkResult instanceRes            = vkCreateInstance(&instanceCI, &Vulkan->Allocator, &Vulkan->Instance);
	if (instanceRes != VK_SUCCESS) { return FALSE; }

	return TRUE;
}

void Vulkan_Shutdown() {
	if (Vulkan->Instance) { vkDestroyInstance(Vulkan->Instance, &Vulkan->Allocator); }

	Free(Vulkan);
}

B8 Vulkan_BeginFrame(F64 deltaTime) {
	return TRUE;
}

B8 Vulkan_EndFrame(F64 deltaTime) {
	return TRUE;
}

void Vulkan_OnResized(I32 w, I32 h) {}
