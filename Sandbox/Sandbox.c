#include <Nara/Nara.h>

int main(int argc, const char** argv) {
	LogF("A critical error has occurred!");
	LogE("Failed to open file for reading: %s", "Somefile.txt");
	LogW("Asset could not be loaded.");
	LogI("Starting subsystem %d of %d...", 5, 32);
	LogD("Value of i: %d", 5);
	LogT("Creating entity component.");

	Assert(5 == 5);
	AssertMsg(1 == 2, "Critical math error!");

	return 0;
}
