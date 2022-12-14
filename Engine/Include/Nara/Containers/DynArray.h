#pragma once

#include <Nara/Defines.h>

static const U32 DynArray_DefaultCapacity = 8;

typedef void** DynArrayT;
typedef const void* const* ConstDynArrayT;

// ===== Internal function implementations =====

/**
 * Create a dynamic array. Users should use the helper macro DynArray_Create() instead of this function directly.
 * @param elementSize The size of each element, in bytes.
 * @param elementCapacity The amount of elements the dynamic array will be able to hold without resizing.
 * @return NULL upon allocation failure, otherwise a pointer to the created dynamic array.
 * @sa DynArray_Create(), DynArray_CreateWithCapacity()
 */
NAPI void* _DynArray_Create(U64 elementSize, U64 elementCapacity);

/**
 * Create a dynamic array with a predetermined size. Users should use the helper macro DynArray_CreateWithSize() instead
 * of this function directly.
 * @param elementSize The size of each element, in bytes.
 * @param elementCount The amount of elements the dynamic array will hold.
 * @return NULL upon allocation failure, otherwise a pointer to the created dynamic array.
 * @sa DynArray_Create(), DynArray_CreateWithCapacity()
 */
NAPI void* _DynArray_CreateSized(U64 elementSize, U64 elementCount);

/**
 * Destroys a dynamic array.
 * @param dynArray The dynamic array to destroy.
 */
NAPI void _DynArray_Destroy(DynArrayT dynArray);

/**
 * Get the dynamic array's capacity.
 * @param dynArray A pointer to the dynamic array.
 * @return The current capacity of the dynamic array.
 */
NAPI U64 _DynArray_Capacity(ConstDynArrayT dynArray);

/**
 * Get the dynamic array's size.
 * @param dynArray A pointer to the dynamic array.
 * @return The current size of the dynamic array.
 */
NAPI U64 _DynArray_Size(ConstDynArrayT dynArray);

/**
 * Get the dynamic array's stride.
 * @param dynArray A pointer to the dynamic array.
 * @return The current stride of the dynamic array.
 */
NAPI U64 _DynArray_Stride(ConstDynArrayT dynArray);

/**
 * Trim the dynamic array, removing any excess capacity.
 * @param dynArray A pointer to the dynamic array.
 * @return TRUE upon successful trim, FALSE otherwise.
 */
NAPI B8 _DynArray_Trim(DynArrayT dynArray);

/**
 * Resize the dynamic array to the specified size.
 * @param dynArray A pointer to the dynamic array to resize.
 * @param elementCount The number of elements to resize to.
 * @return TRUE upon successful resize, FALSE upon failure.
 */
NAPI B8 _DynArray_Resize(DynArrayT dynArray, U64 elementCount);

/**
 * Reserve memory space for the given number of elements.
 * @param dynArray A pointer to the dynamic array to reserve for.
 * @param elementCount The number of elements to reserve for.
 * @return TRUE upon successful reserve, FALSE upon failure.
 */
NAPI B8 _DynArray_Reserve(DynArrayT dynArray, U64 elementCount);

/**
 * Append the given element to the end of the dynamic array.
 * @param dynArray A pointer to the dynamic array to resize.
 * @param element A pointer to the element to append.
 */
NAPI void _DynArray_Push(DynArrayT dynArray, const void* element);

/**
 * Pop the element from the end of the dynamic array.
 * @param dynArray A pointer to the dynamic array.
 * @param[out] element A pointer to where the popped element will be placed.
 */
NAPI void _DynArray_Pop(DynArrayT dynArray, void* element);

/**
 * Insert the given element at the specified index.
 * @param dynArray A pointer to the dynamic array.
 * @param index The index to insert the element into.
 * @param element A pointer to the element to insert.
 */
NAPI void _DynArray_Insert(DynArrayT dynArray, U64 index, const void* element);

/**
 * Extract the element at the specified index and remove it from the array.
 * @param dynArray A pointer to the dynamic array.
 * @param index The index to extract.
 * @param element A pointer to where the extracted element will be placed.
 */
NAPI void _DynArray_Extract(DynArrayT dynArray, U64 index, void* element);

// ===== User-facing macro implementations =====

/**
 * Create a dynamic array.
 * @param type The type the dynamic array will contain.
 * @return The newly created dynamic array.
 */
#define DynArray_Create(type) _DynArray_Create(sizeof(type), DynArray_DefaultCapacity)

/**
 * Create a dynamic array with a specified capacity.
 * @param type The type the dynamic array will contain.
 * @param count The amount of elements the dynamic array will be able to hold without resizing.
 * @return The newly created dynamic array.
 */
#define DynArray_CreateWithCapacity(type, count) _DynArray_Create(sizeof(type), count)

/**
 * Create a dynamic array with a specified size.
 * @param type The type the dynamic array will contain.
 * @param count The amount of elements the dynamic array will hold.
 * @return The newly created dynamic array.
 */
#define DynArray_CreateWithSize(type, count) _DynArray_CreateSized(sizeof(type), count)

/** Convenience macro for _DynArray_Destroy(). */
#define DynArray_Destroy(dynArray) _DynArray_Destroy((DynArrayT) &dynArray)

/** Convenience macro for _DynArray_Capacity(). */
#define DynArray_Capacity(dynArray) _DynArray_Capacity((ConstDynArrayT) &dynArray)

/** Convenience macro for _DynArray_Size(). */
#define DynArray_Size(dynArray) _DynArray_Size((ConstDynArrayT) &dynArray)

/** Convenience macro for _DynArray_Stride(). */
#define DynArray_Stride(dynArray) _DynArray_Stride((ConstDynArrayT) &dynArray)

/** Convenience macro for _DynArray_Trim(). */
#define DynArray_Trim(dynArray) _DynArray_Trim((DynArrayT) &dynArray)

/** Convenience macro for _DynArray_Resize(). */
#define DynArray_Resize(dynArray, elementCount) _DynArray_Resize((DynArrayT) &dynArray, elementCount)

/** Convenience macro for _DynArray_Reserve(). */
#define DynArray_Reserve(dynArray, elementCount) _DynArray_Reserve((DynArrayT) &dynArray, elementCount)

/** Convenience macro for _DynArray_Push(). */
#define DynArray_Push(dynArray, element) _DynArray_Push((DynArrayT) &dynArray, (const void*) &element)

/** Convenience macro for _DynArray_Push(). */
#define DynArray_PushValue(dynArray, element)                      \
	do {                                                             \
		typeof(element) _daTemp = (element);                           \
		_DynArray_Push((DynArrayT) &dynArray, (const void*) &_daTemp); \
	} while (0)

/** Convenience macro for _DynArray_Insert(). */
#define DynArray_Insert(dynArray, index, element) _DynArray_Insert((DynArrayT) &dynArray, index, (const void*) &element)

/** Convenience macro for _DynArray_Insert(). */
#define DynArray_InsertValue(dynArray, index, element)                      \
	do {                                                                      \
		typeof(element) _daTemp = (element);                                    \
		_DynArray_Insert((DynArrayT) &dynArray, index, (const void*) &_daTemp); \
	} while (0)

/** Convenience macro for _DynArray_Pop(). */
#define DynArray_Pop(dynArray, element) _DynArray_Pop((DynArrayT) &dynArray, (void*) element)

/** Convenience macro for _DynArray_Extract(). */
#define DynArray_Extract(dynArray, index, element) _DynArray_Extract((DynArrayT) &dynArray, index, (void*) element)

/** Convenience macro to erase an element without extracting. */
#define DynArray_Erase(dynArray, index) _DynArray_Extract((DynArrayT) &dynArray, index, NULL)
