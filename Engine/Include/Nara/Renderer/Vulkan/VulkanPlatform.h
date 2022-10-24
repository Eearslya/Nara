#pragma once

#include <Nara/Defines.h>
#include <Nara/Renderer/Vulkan/VulkanTypes.h>

void Platform_GetRequiredExtensions(const char*** extensionNames);
VkResult Platform_CreateSurface(const VulkanContext* context, VkSurfaceKHR* surface);
