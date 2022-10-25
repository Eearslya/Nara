#pragma once

#include <Nara/Defines.h>

#ifdef NARA_ASSERTIONS
#	ifdef _MSC_VER
#		include <intrin.h>
#		define NaraDebugBreak() __debugbreak()
#	else
#		define NaraDebugBreak() __builtin_trap()
#	endif

NAPI void Nara_AssertFailure(const char* expr, const char* msg, const char* file, I32 line);

#	define Assert(expr)                                     \
		do {                                                   \
			if (!(expr)) {                                       \
				Nara_AssertFailure(#expr, "", __FILE__, __LINE__); \
				NaraDebugBreak();                                  \
				abort();                                           \
			}                                                    \
		} while (0)
#	define AssertMsg(expr, msg)                              \
		do {                                                    \
			if (!(expr)) {                                        \
				Nara_AssertFailure(#expr, msg, __FILE__, __LINE__); \
				NaraDebugBreak();                                   \
				abort();                                            \
			}                                                     \
		} while (0)

#	if NARA_DEBUG
#		define AssertDebug(expr)                                \
			do {                                                   \
				if (!(expr)) {                                       \
					Nara_AssertFailure(#expr, "", __FILE__, __LINE__); \
					NaraDebugBreak();                                  \
					abort();                                           \
				}                                                    \
			} while (0)
#		define AssertDebugMsg(expr, msg)                         \
			do {                                                    \
				if (!(expr)) {                                        \
					Nara_AssertFailure(#expr, msg, __FILE__, __LINE__); \
					NaraDebugBreak();                                   \
					abort();                                            \
				}                                                     \
			} while (0)
#	else
#		define AssertDebug(expr)
#		define AssertDebugMsg(expr, msg)
#	endif /* NARA_DEBUG */
#else    /* NARA_ASSERTIONS */
#	define Assert(expr)
#	define AssertMsg(expr, msg)
#	define AssertDebug(expr)
#	define AssertDebugMsg(expr, msg)
#endif /* NARA_ASSERTIONS */
