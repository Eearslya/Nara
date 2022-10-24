#include <Nara/Core/Input.h>
#include <Nara/Platform/Platform.h>

#if NARA_WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <Windows.h>
#	include <WindowsX.h>
#	include <stdlib.h>

typedef struct Platform_StateT {
	HINSTANCE Instance;
	HWND Window;
	F64 ClockFrequency;
	LARGE_INTEGER StartTime;
} Platform_State;

static const LPSTR Platform_ClassName = TEXT("Nara");
static Platform_State Platform;

static LRESULT CALLBACK Platform_Message(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam);

B8 Platform_Initialize(const char* appName, I32 startX, I32 startY, U32 width, U32 height) {
	Platform.Instance = GetModuleHandle(NULL);
	Platform.Window   = NULL;

	HICON appIcon     = LoadIcon(Platform.Instance, IDI_APPLICATION);
	HCURSOR appCursor = LoadCursor(NULL, IDC_ARROW);

	WNDCLASSEX wndClass = {.cbSize        = sizeof(WNDCLASSEX),
	                       .style         = CS_DBLCLKS,
	                       .lpfnWndProc   = Platform_Message,
	                       .cbClsExtra    = 0,
	                       .cbWndExtra    = 0,
	                       .hInstance     = Platform.Instance,
	                       .hIcon         = appIcon,
	                       .hCursor       = appCursor,
	                       .hbrBackground = NULL,
	                       .lpszMenuName  = NULL,
	                       .lpszClassName = Platform_ClassName,
	                       .hIconSm       = NULL};
	if (!RegisterClassEx(&wndClass)) {
		MessageBox(NULL, TEXT("Window registration failed!"), TEXT("Fatal Error"), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	const DWORD windowStyle   = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
	const DWORD windowStyleEx = WS_EX_APPWINDOW;

	I32 windowX = startX;
	I32 windowY = startY;
	U32 windowW = width;
	U32 windowH = height;

	RECT borderRect = {0, 0, 0, 0};
	AdjustWindowRectEx(&borderRect, windowStyle, 0, windowStyleEx);
	windowX += borderRect.left;
	windowY += borderRect.top;
	windowW += borderRect.right - borderRect.left;
	windowH += borderRect.bottom - borderRect.top;

	if (startX == -1 && startY == -1) {
		const I32 screenW = GetSystemMetrics(SM_CXSCREEN);
		const I32 screenH = GetSystemMetrics(SM_CYSCREEN);
		windowX           = (screenW / 2) - (windowW / 2);
		windowY           = (screenH / 2) - (windowH / 2);
	}

	Platform.Window = CreateWindowEx(windowStyleEx,
	                                 Platform_ClassName,
	                                 TEXT("Nara"),
	                                 windowStyle,
	                                 windowX,
	                                 windowY,
	                                 windowW,
	                                 windowH,
	                                 NULL,
	                                 NULL,
	                                 Platform.Instance,
	                                 NULL);
	if (Platform.Window == NULL) {
		MessageBoxA(NULL, TEXT("Window creation failed!"), TEXT("Fatal Error"), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(Platform.Window, SW_SHOW);

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	Platform.ClockFrequency = 1.0 / (F64) frequency.QuadPart;
	QueryPerformanceCounter(&Platform.StartTime);

	return 1;
}

void Platform_Shutdown() {
	if (Platform.Window) {
		DestroyWindow(Platform.Window);
		Platform.Window = NULL;
	}
}

B8 Platform_Update() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) { return 0; }
	}

	return 1;
}

void* Platform_Allocate(U64 size, U64 alignment) {
	void** place   = NULL;
	uintptr_t addr = 0;
	void* ptr      = malloc(alignment + size + sizeof(uintptr_t));

	if (ptr == NULL) { return NULL; }

	addr      = ((uintptr_t) ptr + sizeof(uintptr_t) + alignment) & ~(alignment - 1);
	place     = (void**) addr;
	place[-1] = ptr;

	return (void*) addr;
}

void* Platform_Reallocate(void* ptr, U64 size, U64 alignment) {
	if (ptr == NULL) { return Platform_Allocate(size, alignment); }

	void** p = (void**) ptr;
	ptr      = p[-1];

	const U64 actualSize = size + alignment + sizeof(uintptr_t);
	void* newPtr         = realloc(ptr, actualSize);
	uintptr_t userPtr    = ((uintptr_t) newPtr + sizeof(uintptr_t) + alignment) & ~(alignment - 1);
	p                    = (void**) newPtr;
	p[-1]                = newPtr;

	return (void*) userPtr;
}

void Platform_Free(void* ptr) {
	if (ptr != NULL) {
		void** p = (void**) ptr;
		free(p[-1]);
	}
}

void Platform_MemCopy(void* dst, const void* src, U64 size) {
	memcpy(dst, src, size);
}

void Platform_MemMove(void* dst, const void* src, U64 size) {
	memmove(dst, src, size);
}

void Platform_MemSet(void* dst, U8 value, U64 size) {
	memset(dst, value, size);
}

void Platform_MemZero(void* dst, U64 size) {
	Platform_MemSet(dst, 0, size);
}

F64 Platform_GetTime() {
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return (F64) now.QuadPart * Platform.ClockFrequency;
}

void Platform_Sleep(U64 ms) {
	Sleep(ms);
}

static LRESULT CALLBACK Platform_Message(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_MOUSEMOVE: {
			const I32 x = GET_X_LPARAM(lParam);
			const I32 y = GET_Y_LPARAM(lParam);
			Input_OnMouseMoved(x, y);

			return 0;
		} break;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP: {
			const B8 pressed = msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN;
			const Key key    = (U8) wParam;
			Input_OnKey(key, pressed);

			return 0;
		} break;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP: {
			const B8 pressed = msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN;
			const MouseButton button =
				(msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP
			     ? MouseButton_Left
			     : (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP ? MouseButton_Middle : MouseButton_Right));
			Input_OnMouseButton(button, pressed);

			return 0;
		} break;

		case WM_CLOSE: {
			PostQuitMessage(0);

			return 0;
		} break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
#endif /* NARA_WINDOWS */
