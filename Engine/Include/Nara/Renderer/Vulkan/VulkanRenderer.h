#pragma once

#include <Nara/Defines.h>

B8 Vulkan_Initialize();
void Vulkan_Shutdown();
B8 Vulkan_BeginFrame(F64 deltaTime);
B8 Vulkan_EndFrame(F64 deltaTime);
void Vulkan_OnResized(I32 w, I32 h);
