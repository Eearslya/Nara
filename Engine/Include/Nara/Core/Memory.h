#pragma once

#include <Nara/Core/Log.h>
#include <Nara/Defines.h>

typedef enum MemoryTag {
	MemoryTag_Unknown,
	MemoryTag_Array,
	MemoryTag_DynamicArray,
	MemoryTag_Dictionary,
	MemoryTag_RingQueue,
	MemoryTag_BST,
	MemoryTag_String,
	MemoryTag_Application,
	MemoryTag_Job,
	MemoryTag_Texture,
	MemoryTag_MaterialInstance,
	MemoryTag_Renderer,
	MemoryTag_Game,
	MemoryTag_Transform,
	MemoryTag_Entity,
	MemoryTag_EntityNode,
	MemoryTag_Scene,

	MemoryTag_MAX
} MemoryTag;

B8 Memory_Initialize();
void Memory_Shutdown();
NAPI void Memory_LogUsage(LogLevel level);

NAPI void* Allocate(U64 size, MemoryTag tag);
NAPI void* AllocateAligned(U64 size, U64 alignment, MemoryTag tag);
NAPI void* Reallocate(void* ptr, U64 size, MemoryTag tag);
NAPI void* ReallocateAligned(void* ptr, U64 size, U64 alignment, MemoryTag tag);
NAPI void Free(void* ptr);
NAPI void MemCopy(void* dst, const void* src, U64 size);
NAPI void MemMove(void* dst, const void* src, U64 size);
NAPI void MemSet(void* dst, U8 value, U64 size);
NAPI void MemZero(void* dst, U64 size);
