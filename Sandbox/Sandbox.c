#include <Nara/Nara.h>

int main(int argc, const char** argv) {
	if (!Platform_Initialize("Sandbox", -1, -1, 1600, 900)) { return 1; }
	while (Platform_Update()) {}
	Platform_Shutdown();

	return 0;
}
