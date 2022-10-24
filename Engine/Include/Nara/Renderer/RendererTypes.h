#pragma once

#include <Nara/Defines.h>

typedef enum RenderSystem { RenderSystem_Vulkan, RenderSystem_OpenGL, RenderSystem_DirectX } RenderSystem;

typedef struct RenderPacket {
	F64 DeltaTime;
} RenderPacket;
