#include <Nara/Core/Assert.h>
#include <Nara/Core/Log.h>

void Nara_AssertFailure(const char* expr, const char* msg, const char* file, I32 line) {
	LogF("Assertion Failure: %s\n\t%s\n\t%s:%i\n\n", expr, msg, file, line);
}
