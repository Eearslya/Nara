#pragma once

#include <Nara/Defines.h>

#if NARA_WINDOWS
#	define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

typedef struct VulkanFunctions {
	PFN_vkGetInstanceProcAddr GetInstanceProcAddr;

	PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion;
	PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties;
	PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties;
	PFN_vkCreateInstance CreateInstance;

	PFN_vkDestroyInstance DestroyInstance;
	PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
	PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
	PFN_vkDestroySurfaceKHR DestroySurfaceKHR;
#if NARA_WINDOWS
	PFN_vkCreateWin32SurfaceKHR CreateWin32SurfaceKHR;
#endif
} VulkanFunctions;

typedef struct VulkanContext {
	VulkanFunctions vk;

	VkAllocationCallbacks Allocator;
	VkInstance Instance;
#if NARA_DEBUG
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
	VkSurfaceKHR Surface;
} VulkanContext;

extern VulkanContext* Vulkan;
