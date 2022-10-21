#pragma once

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;
typedef signed char I8;
typedef signed short I16;
typedef signed int I32;
typedef signed long long I64;
typedef float F32;
typedef double F64;
typedef unsigned char B8;
typedef int B32;

#if defined(__clang__) || defined(__gcc__)
#	define STATIC_ASSERT _Static_assert
#else
#	define STATIC_ASSET static_assert
#endif

STATIC_ASSERT(sizeof(U8) == 1, "Expected U8 to be 1 byte!");
STATIC_ASSERT(sizeof(U16) == 2, "Expected U16 to be 2 bytes!");
STATIC_ASSERT(sizeof(U32) == 4, "Expected U32 to be 4 bytes!");
STATIC_ASSERT(sizeof(U64) == 8, "Expected U64 to be 8 bytes!");
STATIC_ASSERT(sizeof(I8) == 1, "Expected I8 to be 1 byte!");
STATIC_ASSERT(sizeof(I16) == 2, "Expected I16 to be 2 bytes!");
STATIC_ASSERT(sizeof(I32) == 4, "Expected I32 to be 4 bytes!");
STATIC_ASSERT(sizeof(I64) == 8, "Expected I64 to be 8 bytes!");
STATIC_ASSERT(sizeof(F32) == 4, "Expected F32 to be 4 bytes!");
STATIC_ASSERT(sizeof(F64) == 8, "Expected F64 to be 8 bytes!");
STATIC_ASSERT(sizeof(B8) == 1, "Expected B8 to be 1 byte!");
STATIC_ASSERT(sizeof(B32) == 4, "Expected B32 to be 8 bytes!");
STATIC_ASSERT(sizeof(void*) == 8, "Nara does not support 32-bit platforms!");

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#	define NARA_WINDOWS 1
#elif defined(__linux__) || defined(__gnu_linux__)
#	define NARA_LINUX 1
#	if defined(__ANDROID__)
#		define NARA_ANDROID 1
#	endif
#elif defined(__unix__)
#	define NARA_UNIX
#elif defined(_POSIX_VERSION)
#	define NARA_POSIX 1
#elif defined(__APPLE__)
#	include <TargetConditionals.h>
#	if TARGET_IPHONE_SIMULATOR
#		define NARA_IOS           1
#		define NARA_IOS_SIMULATOR 1
#	elif TARGET_OS_IPHONE
#		define NARA_IOS 1
#	elif TARGET_OS_MAC
#		define NARA_MACOS
#	else
#		error "Unknown Apple platform!"
#	endif
#else
#	error "Unknown platform!"
#endif

#if defined(NARA_BUILD)
#	ifdef _MSC_VER
#		define NAPI __declspec(dllexport)
#	else
#		define NAPI __attribute__((visibility("default")))
#	endif
#else
#	ifdef _MSC_VER
#		define NAPI __declspec(dllimport)
#	else
#		define NAPI
#	endif
#endif
