#pragma once

#include <Nara/Core/Assert.h>
#include <Nara/Core/Log.h>
#include <Nara/Defines.h>

#if NARA_WINDOWS
#	define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

B8 Vulkan_ExtensionPresent(const VkExtensionProperties* available, const char* extension);
B8 Vulkan_TryExtension(const VkExtensionProperties* available, const char** enabled, const char* extension);
const char* Vulkan_ResultToString(VkResult result);

#define VkAssert(call)                                                                                             \
	do {                                                                                                             \
		const VkResult res = call;                                                                                     \
		if (res != VK_SUCCESS) { LogF("[Vulkan] Vulkan call failed with %s: %s", Vulkan_ResultToString(res), #call); } \
	} while (0)

typedef enum Vulkan_QueueType {
	Vulkan_Graphics,
	Vulkan_Transfer,
	Vulkan_Compute,
	Vulkan_QueueType_MAX
} Vulkan_QueueType;

typedef struct VulkanFunctions {
	PFN_vkGetInstanceProcAddr GetInstanceProcAddr;

	PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion;
	PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties;
	PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties;
	PFN_vkCreateInstance CreateInstance;

	PFN_vkCreateDevice CreateDevice;
	PFN_vkDestroyInstance DestroyInstance;
	PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices;
	PFN_vkEnumerateDeviceExtensionProperties EnumerateDeviceExtensionProperties;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties GetPhysicalDeviceQueueFamilyProperties;
	PFN_vkGetPhysicalDeviceFeatures GetPhysicalDeviceFeatures;
	PFN_vkGetPhysicalDeviceFeatures2 GetPhysicalDeviceFeatures2;
	PFN_vkGetPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties;
	PFN_vkGetPhysicalDeviceProperties GetPhysicalDeviceProperties;
	PFN_vkGetPhysicalDeviceProperties2 GetPhysicalDeviceProperties2;
	PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
	PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
	PFN_vkDestroySurfaceKHR DestroySurfaceKHR;
#if NARA_WINDOWS
	PFN_vkCreateWin32SurfaceKHR CreateWin32SurfaceKHR;
#endif

	PFN_vkGetDeviceProcAddr GetDeviceProcAddr;
	PFN_vkDestroyDevice DestroyDevice;
	PFN_vkGetDeviceQueue GetDeviceQueue;
} VulkanFunctions;

typedef struct VulkanDevice_Extensions {
	B8 CalibratedTimestamps;
	B8 DebugUtils;
	B8 GetSurfaceCapabilities2;
	B8 Maintenance4;
	B8 Surface;
	B8 Swapchain;
	B8 Synchronization2;
	B8 TimelineSemaphore;
	B8 ValidationFeatures;
} VulkanDevice_Extensions;

typedef struct VulkanDevice_Features {
	VkPhysicalDeviceFeatures Features;
	VkPhysicalDeviceMaintenance4Features Maintenance4;
#ifdef VK_ENABLE_BETA_EXTENSIONS
	VkPhysicalDevicePortabilitySubsetFeaturesKHR PortabilitySubset;
#endif
	VkPhysicalDeviceShaderDrawParametersFeatures ShaderDrawParameters;
	VkPhysicalDeviceSynchronization2Features Synchronization2;
	VkPhysicalDeviceTimelineSemaphoreFeatures TimelineSemaphore;
} VulkanDevice_Features;

typedef struct VulkanDevice_Properties {
	VkPhysicalDeviceProperties Properties;
	VkPhysicalDeviceDriverProperties Driver;
	VkPhysicalDeviceMaintenance4Properties Maintenance4;
#ifdef VK_ENABLE_BETA_EXTENSIONS
	VkPhysicalDevicePortabilitySubsetPropertiesKHR PortabilitySubset;
#endif
	VkPhysicalDeviceTimelineSemaphoreProperties TimelineSemaphore;
} VulkanDevice_Properties;

typedef struct VulkanDevice_Queue {
	U32 Family;
	U32 Index;
	VkQueue Queue;
} VulkanDevice_Queue;

typedef struct VulkanDevice_Info {
	VkExtensionProperties* AvailableExtensions;  // DynArray
	VulkanDevice_Features AvailableFeatures;
	VkPhysicalDeviceMemoryProperties Memory;
	VulkanDevice_Properties Properties;
	VkQueueFamilyProperties* QueueFamilies;  // DynArray

	const char** EnabledExtensions;  // DynArray
	VulkanDevice_Features EnabledFeatures;
} VulkanDevice_Info;

struct VulkanDevice_T {
	VkPhysicalDevice PhysicalDevice;
	VkDevice Device;
};
typedef struct VulkanDevice_T* VulkanDevice;

struct VulkanContext_T {
	VulkanFunctions vk;

	VkAllocationCallbacks Allocator;
	VkInstance Instance;
#if NARA_DEBUG
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
	VkSurfaceKHR Surface;
	VulkanDevice_Info DeviceInfo;
	VulkanDevice_Extensions Extensions;
	VulkanDevice_Queue Queues[Vulkan_QueueType_MAX];
	VulkanDevice Device;
};
typedef struct VulkanContext_T* VulkanContext;

extern VulkanContext Vulkan;
