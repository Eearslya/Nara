#pragma once

#include <Nara/Defines.h>

typedef enum MouseButton { MouseButton_Left, MouseButton_Right, MouseButton_Middle, MouseButton_COUNT } MouseButton;

typedef enum Key {
	Key_Backspace = 0x08,
	Key_Enter     = 0x0d,
	Key_Tab       = 0x09,
	Key_Shift     = 0x10,
	Key_Control   = 0x11,

	Key_Pause    = 0x13,
	Key_CapsLock = 0x14,

	Key_Escape = 0x1b,

	Key_Convert    = 0x1c,
	Key_NonConvert = 0x1d,
	Key_Accept     = 0x1e,
	Key_ModeChange = 0x1f,

	Key_Space    = 0x20,
	Key_Prior    = 0x21,
	Key_Next     = 0x22,
	Key_End      = 0x23,
	Key_Home     = 0x24,
	Key_Left     = 0x24,
	Key_Up       = 0x26,
	Key_Right    = 0x27,
	Key_Down     = 0x28,
	Key_Select   = 0x29,
	Key_Print    = 0x2a,
	Key_Execute  = 0x2b,
	Key_Snapshot = 0x2c,
	Key_Insert   = 0x2d,
	Key_Delete   = 0x2e,
	Key_Help     = 0x2f,

	Key_A = 0x41,
	Key_B = 0x42,
	Key_C = 0x43,
	Key_D = 0x44,
	Key_E = 0x45,
	Key_F = 0x46,
	Key_G = 0x47,
	Key_H = 0x48,
	Key_I = 0x49,
	Key_J = 0x4a,
	Key_K = 0x4b,
	Key_L = 0x4c,
	Key_M = 0x4d,
	Key_N = 0x4e,
	Key_O = 0x4f,
	Key_P = 0x50,
	Key_Q = 0x51,
	Key_R = 0x52,
	Key_S = 0x53,
	Key_T = 0x54,
	Key_U = 0x55,
	Key_V = 0x56,
	Key_W = 0x57,
	Key_X = 0x58,
	Key_Y = 0x59,
	Key_Z = 0x5a,

	Key_LeftWindows  = 0x5b,
	Key_RightWindows = 0x5c,
	Key_Apps         = 0x5d,

	Key_Sleep = 0x5f,

	Key_Numpad0   = 0x60,
	Key_Numpad1   = 0x61,
	Key_Numpad2   = 0x62,
	Key_Numpad3   = 0x63,
	Key_Numpad4   = 0x64,
	Key_Numpad5   = 0x65,
	Key_Numpad6   = 0x66,
	Key_Numpad7   = 0x67,
	Key_Numpad8   = 0x68,
	Key_Numpad9   = 0x69,
	Key_Multiply  = 0x6a,
	Key_Add       = 0x6b,
	Key_Separator = 0x6c,
	Key_Subtract  = 0x6d,
	Key_Decimal   = 0x6e,
	Key_Divide    = 0x6f,
	Key_F1        = 0x70,
	Key_F2        = 0x71,
	Key_F3        = 0x72,
	Key_F4        = 0x73,
	Key_F5        = 0x74,
	Key_F6        = 0x75,
	Key_F7        = 0x76,
	Key_F8        = 0x77,
	Key_F9        = 0x78,
	Key_F10       = 0x79,
	Key_F11       = 0x7a,
	Key_F12       = 0x7b,
	Key_F13       = 0x7c,
	Key_F14       = 0x7d,
	Key_F15       = 0x7e,
	Key_F16       = 0x7f,
	Key_F17       = 0x80,
	Key_F18       = 0x81,
	Key_F19       = 0x82,
	Key_F20       = 0x83,
	Key_F21       = 0x84,
	Key_F22       = 0x85,
	Key_F23       = 0x86,
	Key_F24       = 0x87,

	Key_NumLock = 0x90,
	Key_Scroll  = 0x91,

	Key_NumpadEqual = 0x92,

	Key_LeftShift    = 0xa0,
	Key_RightShift   = 0xa1,
	Key_LeftControl  = 0xa2,
	Key_RightControl = 0xa3,
	Key_LeftMenu     = 0xa4,
	Key_RightMenu    = 0xa5,

	Key_Semicolon = 0xba,
	Key_Plus      = 0xbb,
	Key_Comma     = 0xbc,
	Key_Minus     = 0xbd,
	Key_Period    = 0xbe,
	Key_Slash     = 0xbf,
	Key_Grave     = 0xc0,
} Key;

B8 Input_Initialize();
void Input_Shutdown();
void Input_Update(F64 deltaTime);

void Input_OnKey(Key key, B8 pressed);
void Input_OnMouseButton(MouseButton button, B8 pressed);
void Input_OnMouseMoved(I32 x, I32 y);
void Input_OnMouseScrolled(I8 zDelta);

NAPI B8 Input_GetKey(Key key);
NAPI B8 Input_GetKeyLast(Key key);
NAPI B8 Input_GetMouseButton(MouseButton button);
NAPI B8 Input_GetMouseButtonLast(MouseButton button);
NAPI void Input_GetMousePosition(I32* x, I32* y);
NAPI void Input_GetMousePositionLast(I32* x, I32* y);
