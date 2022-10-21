#include <Nara/Core/Log.h>
#include <Nara/Core/Memory.h>
#include <Nara/Platform/Platform.h>
#include <stdlib.h>

static const char* MemoryTag_Names[] = {"Unknown",
                                        "Array",
                                        "DynArray",
                                        "Dictionary",
                                        "RingQueue",
                                        "BST",
                                        "String",
                                        "Application",
                                        "Job",
                                        "Texture",
                                        "MaterialInstance",
                                        "Renderer",
                                        "Game",
                                        "Transform",
                                        "Entity",
                                        "EntityNode",
                                        "Scene"};

struct MemoryState {
	U64 TotalAllocations;
	U64 TotalBytes;

	U64 AllocationsByTag[MemoryTag_MAX];
	U64 BytesByTag[MemoryTag_MAX];
} Memory;

typedef struct MemoryHeader {
	void* Original;
	U64 Size;
	U64 Tag;
} MemoryHeader;

B8 Memory_Initialize() {
	Platform_MemZero(&Memory, sizeof(Memory));

	return 1;
}

void Memory_Shutdown() {
	if (Memory.TotalAllocations != 0) {
		LogW("[Memory] Memory subsystem is shutting down with %llu bytes still allocated across %llu allocations!",
		     Memory.TotalBytes,
		     Memory.TotalAllocations);
		Memory_LogUsage(LogLevel_Warn);
	}
}

void Memory_LogUsage(LogLevel level) {
	Log_Output(level, "[Memory] Current Memory subsystem statistics:");
	Log_Output(level, "[Memory]   Total Allocations: %llu", Memory.TotalAllocations);
	Log_Output(level, "[Memory]   Total Bytes: %llu", Memory.TotalBytes);
	if (Memory.TotalAllocations > 0) {
		Log_Output(level, "[Memory]  By Tag:");
		for (int i = 0; i < MemoryTag_MAX; ++i) {
			if (Memory.AllocationsByTag[i] > 0) {
				Log_Output(level,
				           "[Memory]    %s: %llu Allocations, %llu Bytes",
				           MemoryTag_Names[i],
				           Memory.AllocationsByTag[i],
				           Memory.BytesByTag[i]);
			}
		}
	}
}

void* Allocate(U64 size, MemoryTag tag) {
	return AllocateAligned(size, 1, tag);
}

void* AllocateAligned(U64 size, U64 alignment, MemoryTag tag) {
	const U64 padding    = alignment > sizeof(MemoryHeader) ? alignment : sizeof(MemoryHeader);
	const U64 actualSize = size + padding;

	void* ret = NULL;
	void* ptr = Platform_Allocate(actualSize, alignment);
	if (ptr != NULL) {
		ret                  = ptr + padding;
		MemoryHeader* header = &((MemoryHeader*) ret)[-1];
		header->Original     = ptr;
		header->Size         = size;
		header->Tag          = tag;

		Memory.TotalAllocations++;
		Memory.TotalBytes += size;
		Memory.AllocationsByTag[tag]++;
		Memory.BytesByTag[tag] += size;
	}

	return ret;
}

void Free(void* ptr) {
	if (ptr == NULL) { return; }

	MemoryHeader* header = &((MemoryHeader*) ptr)[-1];
	Memory.TotalAllocations--;
	Memory.TotalBytes -= header->Size;
	Memory.AllocationsByTag[header->Tag]--;
	Memory.BytesByTag[header->Tag] -= header->Size;

	Platform_Free(header->Original);
}

void MemCopy(void* dst, const void* src, U64 size) {
	Platform_MemCopy(dst, src, size);
}

void MemSet(void* dst, U8 value, U64 size) {
	Platform_MemSet(dst, value, size);
}

void MemZero(void* dst, U64 size) {
	Platform_MemZero(dst, size);
}
