#pragma once

#include <Nara/Defines.h>
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
} VulkanFunctions;

typedef struct VulkanContext {
	VulkanFunctions vk;

	VkAllocationCallbacks Allocator;
	VkInstance Instance;
#if NARA_DEBUG
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
} VulkanContext;
