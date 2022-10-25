#include <Nara/Containers/String.h>
#include <Nara/Core/Memory.h>

char* String_Duplicate(const char* str) {
	const U64 length = String_Length(str);
	char* dup        = Allocate(length + 1, MemoryTag_String);
	MemCopy(dup, str, length + 1);

	return dup;
}

B8 String_Equal(const char* a, const char* b) {
	const U64 lengthA = String_Length(a);
	const U64 lengthB = String_Length(b);
	if (lengthA != lengthB) { return FALSE; }

	for (U64 i = 0; i < lengthA; ++i) {
		if (a[i] != b[i]) { return FALSE; }
	}

	return TRUE;
}

U64 String_Length(const char* str) {
	U64 size = 0;
	for (; str[size] != 0; ++size) {}

	return size;
}
