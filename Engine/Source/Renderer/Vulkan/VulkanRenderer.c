#include <Nara/Containers/DynArray.h>
#include <Nara/Containers/String.h>
#include <Nara/Core/Log.h>
#include <Nara/Core/Memory.h>
#include <Nara/Renderer/Vulkan/VulkanDevice.h>
#include <Nara/Renderer/Vulkan/VulkanPlatform.h>
#include <Nara/Renderer/Vulkan/VulkanRenderer.h>
#include <Nara/Renderer/Vulkan/VulkanTypes.h>

static VulkanContext Vulkan = NULL;

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

const char* Vulkan_ResultToString(VkResult result) {
#define Result(res) \
	case res:         \
		return #res

	switch (result) {
		Result(VK_SUCCESS);
		Result(VK_NOT_READY);
		Result(VK_TIMEOUT);
		Result(VK_EVENT_SET);
		Result(VK_EVENT_RESET);
		Result(VK_INCOMPLETE);
		Result(VK_ERROR_OUT_OF_HOST_MEMORY);
		Result(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		Result(VK_ERROR_INITIALIZATION_FAILED);
		Result(VK_ERROR_DEVICE_LOST);
		Result(VK_ERROR_MEMORY_MAP_FAILED);
		Result(VK_ERROR_LAYER_NOT_PRESENT);
		Result(VK_ERROR_EXTENSION_NOT_PRESENT);
		Result(VK_ERROR_FEATURE_NOT_PRESENT);
		Result(VK_ERROR_INCOMPATIBLE_DRIVER);
		Result(VK_ERROR_TOO_MANY_OBJECTS);
		Result(VK_ERROR_FORMAT_NOT_SUPPORTED);
		Result(VK_ERROR_FRAGMENTED_POOL);
		Result(VK_ERROR_UNKNOWN);
		Result(VK_ERROR_OUT_OF_POOL_MEMORY);
		Result(VK_ERROR_INVALID_EXTERNAL_HANDLE);
		Result(VK_ERROR_FRAGMENTATION);
		Result(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
		Result(VK_PIPELINE_COMPILE_REQUIRED);
		Result(VK_ERROR_SURFACE_LOST_KHR);
		Result(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
		Result(VK_SUBOPTIMAL_KHR);
		Result(VK_ERROR_OUT_OF_DATE_KHR);
		Result(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
		Result(VK_ERROR_VALIDATION_FAILED_EXT);
		Result(VK_ERROR_INVALID_SHADER_NV);
		Result(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
		Result(VK_ERROR_NOT_PERMITTED_KHR);
		Result(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
		Result(VK_THREAD_IDLE_KHR);
		Result(VK_THREAD_DONE_KHR);
		Result(VK_OPERATION_DEFERRED_KHR);
		Result(VK_OPERATION_NOT_DEFERRED_KHR);
		default:
			return "UNKNOWN ERROR";
	}

#undef Result
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
	Load(CreateDevice);
	Load(DestroyInstance);
	Load(EnumeratePhysicalDevices);
	Load(EnumerateDeviceExtensionProperties);
	Load(GetPhysicalDeviceQueueFamilyProperties);
	Load(GetPhysicalDeviceFeatures);
	Load(GetPhysicalDeviceFeatures2);
	Load(GetPhysicalDeviceMemoryProperties);
	Load(GetPhysicalDeviceProperties);
	Load(GetPhysicalDeviceProperties2);
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

	const VkApplicationInfo appInfo = {.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
	                                   .pNext              = NULL,
	                                   .pApplicationName   = "Nara",
	                                   .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
	                                   .pEngineName        = "Nara",
	                                   .engineVersion      = VK_MAKE_API_VERSION(0, 1, 0, 0),
	                                   .apiVersion         = VK_API_VERSION_1_2};

	VkInstanceCreateInfo instanceCI = {.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	                                   .pNext                   = NULL,
	                                   .flags                   = 0,
	                                   .pApplicationInfo        = &appInfo,
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

	if (!VulkanDevice_Initialize()) { return FALSE; }

	return TRUE;
}

void Vulkan_Shutdown() {
	VulkanDevice_Shutdown();

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

B8 Vulkan_ExtensionPresent(const VkExtensionProperties* available, const char* name) {
	const U64 availableCount = DynArray_Size(available);
	for (U64 i = 0; i < availableCount; ++i) {
		if (String_Equal(available[i].extensionName, name)) { return TRUE; }
	}

	return FALSE;
}

B8 Vulkan_TryExtension(const VkExtensionProperties* available, const char** enabled, const char* extension) {
	if (!Vulkan_ExtensionPresent(available, extension)) { return FALSE; }

	const int enabledCount = DynArray_Size(enabled);
	for (int i = 0; i < enabledCount; ++i) {
		if (String_Equal(enabled[i], extension)) { return TRUE; }
	}

	DynArray_PushValue(enabled, extension);

	return TRUE;
}
