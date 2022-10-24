#include <Nara/Core/Event.h>
#include <Nara/Core/Input.h>
#include <Nara/Core/Memory.h>

typedef struct KeyboardState {
	B8 Keys[256];
} KeyboardState;

typedef struct MouseState {
	I32 X;
	I32 Y;
	B8 Buttons[32];
} MouseState;

struct Input_State {
	KeyboardState Keyboard;
	KeyboardState KeyboardLast;
	MouseState Mouse;
	MouseState MouseLast;
};

static B8 Input_Initialized = FALSE;
static struct Input_State Input;

B8 Input_Initialize() {
	MemZero(&Input, sizeof(Input));

	Input_Initialized = TRUE;

	return TRUE;
}

void Input_Shutdown() {
	Input_Initialized = FALSE;
}

void Input_Update(F64 deltaTime) {
	if (!Input_Initialized) { return; }

	MemCopy(&Input.KeyboardLast, &Input.Keyboard, sizeof(Input.Keyboard));
	MemCopy(&Input.MouseLast, &Input.Mouse, sizeof(Input.Mouse));
}

void Input_OnKey(Key key, B8 pressed) {
	if (!Input_Initialized) { return; }

	if (Input.Keyboard.Keys[key] != pressed) {
		Input.Keyboard.Keys[key] = pressed;

		const EventContext context = {.Data = {.U8[0] = key}};
		Event_Fire(pressed ? Event_KeyPressed : Event_KeyReleased, NULL, context);
	}
}

void Input_OnMouseButton(MouseButton button, B8 pressed) {
	if (!Input_Initialized) { return; }

	if (Input.Mouse.Buttons[button] != pressed) {
		Input.Mouse.Buttons[button] = pressed;

		const EventContext context = {.Data = {.U8[0] = button}};
		Event_Fire(pressed ? Event_MouseButtonPressed : Event_MouseButtonReleased, NULL, context);
	}
}

void Input_OnMouseMoved(I32 x, I32 y) {
	if (!Input_Initialized) { return; }

	if (Input.Mouse.X != x || Input.Mouse.Y != y) {
		Input.Mouse.X = x;
		Input.Mouse.Y = y;

		const EventContext context = {.Data = {.I32[0] = x, .I32[1] = y}};
		Event_Fire(Event_MouseMoved, NULL, context);
	}
}

void Input_OnMouseScrolled(I8 zDelta) {
	if (!Input_Initialized) { return; }

	const EventContext context = {.Data = {.I8[0] = zDelta}};
	Event_Fire(Event_MouseScrolled, NULL, context);
}

B8 Input_GetKey(Key key) {
	if (!Input_Initialized) { return FALSE; }

	return Input.Keyboard.Keys[key];
}

B8 Input_GetKeyLast(Key key) {
	if (!Input_Initialized) { return FALSE; }

	return Input.KeyboardLast.Keys[key];
}

B8 Input_GetMouseButton(MouseButton button) {
	if (!Input_Initialized) { return FALSE; }

	return Input.Mouse.Buttons[button];
}

B8 Input_GetMouseButtonLast(MouseButton button) {
	if (!Input_Initialized) { return FALSE; }

	return Input.MouseLast.Buttons[button];
}

void Input_GetMousePosition(I32* x, I32* y) {
	if (!Input_Initialized) {
		*x = 0;
		*y = 0;
	}

	*x = Input.Mouse.X;
	*y = Input.Mouse.Y;
}

void Input_GetMousePositionLast(I32* x, I32* y) {
	if (!Input_Initialized) {
		*x = 0;
		*y = 0;
	}

	*x = Input.MouseLast.X;
	*y = Input.MouseLast.Y;
}
