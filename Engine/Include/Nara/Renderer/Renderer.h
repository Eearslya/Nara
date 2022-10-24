#pragma once

#include <Nara/Defines.h>
#include <Nara/Renderer/RendererTypes.h>

B8 Renderer_Initialize();
void Renderer_Shutdown();

B8 Renderer_DrawFrame(const RenderPacket* packet);
