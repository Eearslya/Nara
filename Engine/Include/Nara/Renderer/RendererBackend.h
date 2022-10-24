#pragma once

#include <Nara/Defines.h>
#include <Nara/Renderer/RendererTypes.h>

B8 RendererBackend_Initialize(RenderSystem system);
void RendererBackend_Shutdown();

B8 RendererBackend_BeginFrame(F64 deltaTime);
B8 RendererBackend_EndFrame(F64 deltaTime);

void RendererBackend_OnResized(I32 w, I32 h);
