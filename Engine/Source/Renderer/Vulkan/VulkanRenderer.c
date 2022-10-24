#include <Nara/Containers/DynArray.h>
#include <Nara/Core/Log.h>
#include <Nara/Core/Memory.h>
#include <Nara/Renderer/Vulkan/VulkanPlatform.h>
#include <Nara/Renderer/Vulkan/VulkanRenderer.h>
#include <Nara/Renderer/Vulkan/VulkanTypes.h>

static VulkanContext* Vulkan = NULL;

static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                           VkDebugUtilsMessageTypeFlagsEXT types,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* data,
                                                           void* userData) {
	switch (severity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			LogE("[Vulkan] Validation Error: %s", data->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			LogW("[Vulkan] Validation Warning: %s", data->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			LogD("[Vulkan] Validation Info: %s", data->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			LogD("[Vulkan] Validation: %s", data->pMessage);
			break;

		default:
			break;
	}

	return VK_FALSE;
}

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

void Vulkan_LoadGlobal() {
	Vulkan->vk.GetInstanceProcAddr = vkGetInstanceProcAddr;

#define Load(fn) Vulkan->vk.fn = (PFN_vk##fn) Vulkan->vk.GetInstanceProcAddr(NULL, "vk" #fn)
	Load(EnumerateInstanceVersion);
	Load(EnumerateInstanceExtensionProperties);
	Load(EnumerateInstanceLayerProperties);
	Load(CreateInstance);
#undef Load
}

void Vulkan_LoadInstance() {
#define Load(fn) Vulkan->vk.fn = (PFN_vk##fn) Vulkan->vk.GetInstanceProcAddr(Vulkan->Instance, "vk" #fn)
	Load(DestroyInstance);
	Load(CreateDebugUtilsMessengerEXT);
	Load(DestroyDebugUtilsMessengerEXT);
	Load(DestroySurfaceKHR);
#if NARA_WINDOWS
	Load(CreateWin32SurfaceKHR);
#endif
#undef Load
}

B8 Vulkan_Initialize() {
	Vulkan = Allocate(sizeof(*Vulkan), MemoryTag_Renderer);
	MemZero(Vulkan, sizeof(*Vulkan));

	Vulkan->Allocator.pfnAllocation   = Vulkan_Allocate;
	Vulkan->Allocator.pfnReallocation = Vulkan_Reallocate;
	Vulkan->Allocator.pfnFree         = Vulkan_Free;

	Vulkan_LoadGlobal();

	const char** requiredExtensions = DynArray_Create(const char*);
	DynArray_PushValue(requiredExtensions, &VK_KHR_SURFACE_EXTENSION_NAME);
	Platform_GetRequiredExtensions(&requiredExtensions);
#if NARA_DEBUG
	DynArray_PushValue(requiredExtensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	LogD("[Vulkan] Required Instance extensions:");
	for (int i = 0; i < DynArray_Size(requiredExtensions); ++i) { LogD("[Vulkan] - %s", requiredExtensions[i]); }

	const char** requiredLayers = DynArray_Create(const char*);
#if NARA_DEBUG
	DynArray_PushValue(requiredLayers, &"VK_LAYER_KHRONOS_validation");
#endif

	LogD("[Vulkan] Required Instance layers:");
	for (int i = 0; i < DynArray_Size(requiredLayers); ++i) { LogD("[Vulkan] - %s", requiredLayers[i]); }

	VkInstanceCreateInfo instanceCI = {.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	                                   .pNext                   = NULL,
	                                   .flags                   = 0,
	                                   .pApplicationInfo        = NULL,
	                                   .enabledLayerCount       = DynArray_Size(requiredLayers),
	                                   .ppEnabledLayerNames     = requiredLayers,
	                                   .enabledExtensionCount   = DynArray_Size(requiredExtensions),
	                                   .ppEnabledExtensionNames = requiredExtensions};

#if NARA_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT instanceDebugCI = {
		.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext           = instanceCI.pNext,
		.flags           = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
		.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
	                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		.pfnUserCallback = Vulkan_DebugCallback,
		.pUserData       = NULL};
	instanceCI.pNext = &instanceDebugCI;
#endif

	const VkResult instanceRes = Vulkan->vk.CreateInstance(&instanceCI, &Vulkan->Allocator, &Vulkan->Instance);

	DynArray_Destroy(requiredLayers);
	DynArray_Destroy(requiredExtensions);
	if (instanceRes != VK_SUCCESS) { return FALSE; }

	Vulkan_LoadInstance();

#if NARA_DEBUG
	const VkDebugUtilsMessengerCreateInfoEXT messengerCI = {
		.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext           = NULL,
		.flags           = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
		.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
	                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		.pfnUserCallback = Vulkan_DebugCallback,
		.pUserData       = NULL};
	const VkResult messengerRes = Vulkan->vk.CreateDebugUtilsMessengerEXT(
		Vulkan->Instance, &messengerCI, &Vulkan->Allocator, &Vulkan->DebugMessenger);
	if (messengerRes != VK_SUCCESS) { return FALSE; }
#endif

	const VkResult surfaceRes = Platform_CreateSurface(Vulkan, &Vulkan->Surface);
	if (surfaceRes != VK_SUCCESS) { return FALSE; }

	return TRUE;
}

void Vulkan_Shutdown() {
	if (Vulkan->Instance) {
		if (Vulkan->Surface) { Vulkan->vk.DestroySurfaceKHR(Vulkan->Instance, Vulkan->Surface, &Vulkan->Allocator); }
#if NARA_DEBUG
		if (Vulkan->DebugMessenger) {
			Vulkan->vk.DestroyDebugUtilsMessengerEXT(Vulkan->Instance, Vulkan->DebugMessenger, &Vulkan->Allocator);
		}
#endif

		Vulkan->vk.DestroyInstance(Vulkan->Instance, &Vulkan->Allocator);
	}

	Free(Vulkan);
}

B8 Vulkan_BeginFrame(F64 deltaTime) {
	return TRUE;
}

B8 Vulkan_EndFrame(F64 deltaTime) {
	return TRUE;
}

void Vulkan_OnResized(I32 w, I32 h) {}
