#pragma once

#include <glm/vec2.hpp>





namespace Engine {

    #ifdef ENGINE_USE_GLFW
        #define ENGINE_KEY_UNKNOWN GLFW_KEY_UNKNOWN
        #define ENGINE_KEY_SPACE GLFW_KEY_SPACE
        #define ENGINE_KEY_APOSTROPHE GLFW_KEY_APOSTROPHE
        #define ENGINE_KEY_COMMA GLFW_KEY_COMMA
        #define ENGINE_KEY_MINUS GLFW_KEY_MINUS
        #define ENGINE_KEY_PERIOD GLFW_KEY_PERIOD
        #define ENGINE_KEY_SLASH GLFW_KEY_SLASH
        #define ENGINE_KEY_0 GLFW_KEY_0
        #define ENGINE_KEY_1 GLFW_KEY_1
        #define ENGINE_KEY_2 GLFW_KEY_2
        #define ENGINE_KEY_3 GLFW_KEY_3
        #define ENGINE_KEY_4 GLFW_KEY_4
        #define ENGINE_KEY_5 GLFW_KEY_5
        #define ENGINE_KEY_6 GLFW_KEY_6
        #define ENGINE_KEY_7 GLFW_KEY_7
        #define ENGINE_KEY_8 GLFW_KEY_8
        #define ENGINE_KEY_9 GLFW_KEY_9
        #define ENGINE_KEY_SEMICOLON GLFW_KEY_SEMICOLON
        #define ENGINE_KEY_EQUAL GLFW_KEY_EQUAL
        #define ENGINE_KEY_A GLFW_KEY_A
        #define ENGINE_KEY_B GLFW_KEY_B
        #define ENGINE_KEY_C GLFW_KEY_C
        #define ENGINE_KEY_D GLFW_KEY_D
        #define ENGINE_KEY_E GLFW_KEY_E
        #define ENGINE_KEY_F GLFW_KEY_F
        #define ENGINE_KEY_G GLFW_KEY_G
        #define ENGINE_KEY_H GLFW_KEY_H
        #define ENGINE_KEY_I GLFW_KEY_I
        #define ENGINE_KEY_J GLFW_KEY_J
        #define ENGINE_KEY_K GLFW_KEY_K
        #define ENGINE_KEY_L GLFW_KEY_L
        #define ENGINE_KEY_M GLFW_KEY_M
        #define ENGINE_KEY_N GLFW_KEY_N
        #define ENGINE_KEY_O GLFW_KEY_O
        #define ENGINE_KEY_P GLFW_KEY_P
        #define ENGINE_KEY_Q GLFW_KEY_Q
        #define ENGINE_KEY_R GLFW_KEY_R
        #define ENGINE_KEY_S GLFW_KEY_S
        #define ENGINE_KEY_T GLFW_KEY_T
        #define ENGINE_KEY_U GLFW_KEY_U
        #define ENGINE_KEY_V GLFW_KEY_V
        #define ENGINE_KEY_W GLFW_KEY_W
        #define ENGINE_KEY_X GLFW_KEY_X
        #define ENGINE_KEY_Y GLFW_KEY_Y
        #define ENGINE_KEY_Z GLFW_KEY_Z
        #define ENGINE_KEY_LEFT_BRACKET GLFW_KEY_LEFT_BRACKET
        #define ENGINE_KEY_BACKSLASH GLFW_KEY_BACKSLASH
        #define ENGINE_KEY_RIGHT_BRACKET GLFW_KEY_RIGHT_BRACKET
        #define ENGINE_KEY_GRAVE_ACCENT GLFW_KEY_GRAVE_ACCENT
        #define ENGINE_KEY_WORLD_1 GLFW_KEY_WORLD_1
        #define ENGINE_KEY_WORLD_2 GLFW_KEY_WORLD_2
        #define ENGINE_KEY_ESCAPE GLFW_KEY_ESCAPE
        #define ENGINE_KEY_ENTER GLFW_KEY_ENTER
        #define ENGINE_KEY_TAB GLFW_KEY_TAB
        #define ENGINE_KEY_BACKSPACE GLFW_KEY_BACKSPACE
        #define ENGINE_KEY_INSERT GLFW_KEY_INSERT
        #define ENGINE_KEY_DELETE GLFW_KEY_DELETE
        #define ENGINE_KEY_RIGHT GLFW_KEY_RIGHT
        #define ENGINE_KEY_LEFT GLFW_KEY_LEFT
        #define ENGINE_KEY_DOWN GLFW_KEY_DOWN
        #define ENGINE_KEY_UP GLFW_KEY_UP
        #define ENGINE_KEY_PAGE_UP GLFW_KEY_PAGE_UP
        #define ENGINE_KEY_PAGE_DOWN GLFW_KEY_PAGE_DOWN
        #define ENGINE_KEY_HOME GLFW_KEY_HOME
        #define ENGINE_KEY_END GLFW_KEY_END
        #define ENGINE_KEY_CAPS_LOCK GLFW_KEY_CAPS_LOCK
        #define ENGINE_KEY_SCROLL_LOCK GLFW_KEY_SCROLL_LOCK
        #define ENGINE_KEY_NUM_LOCK GLFW_KEY_NUM_LOCK
        #define ENGINE_KEY_PRINT_SCREEN GLFW_KEY_PRINT_SCREEN
        #define ENGINE_KEY_PAUSE GLFW_KEY_PAUSE
        #define ENGINE_KEY_F1 GLFW_KEY_F1
        #define ENGINE_KEY_F2 GLFW_KEY_F2
        #define ENGINE_KEY_F3 GLFW_KEY_F3
        #define ENGINE_KEY_F4 GLFW_KEY_F4
        #define ENGINE_KEY_F5 GLFW_KEY_F5
        #define ENGINE_KEY_F6 GLFW_KEY_F6
        #define ENGINE_KEY_F7 GLFW_KEY_F7
        #define ENGINE_KEY_F8 GLFW_KEY_F8
        #define ENGINE_KEY_F9 GLFW_KEY_F9
        #define ENGINE_KEY_F10 GLFW_KEY_F10
        #define ENGINE_KEY_F11 GLFW_KEY_F11
        #define ENGINE_KEY_F12 GLFW_KEY_F12
        #define ENGINE_KEY_F13 GLFW_KEY_F13
        #define ENGINE_KEY_F14 GLFW_KEY_F14
        #define ENGINE_KEY_F15 GLFW_KEY_F15
        #define ENGINE_KEY_F16 GLFW_KEY_F16
        #define ENGINE_KEY_F17 GLFW_KEY_F17
        #define ENGINE_KEY_F18 GLFW_KEY_F18
        #define ENGINE_KEY_F19 GLFW_KEY_F19
        #define ENGINE_KEY_F20 GLFW_KEY_F20
        #define ENGINE_KEY_F21 GLFW_KEY_F21
        #define ENGINE_KEY_F22 GLFW_KEY_F22
        #define ENGINE_KEY_F23 GLFW_KEY_F23
        #define ENGINE_KEY_F24 GLFW_KEY_F24
        #define ENGINE_KEY_F25 GLFW_KEY_F25
        #define ENGINE_KEY_KP_0 GLFW_KEY_KP_0
        #define ENGINE_KEY_KP_1 GLFW_KEY_KP_1
        #define ENGINE_KEY_KP_2 GLFW_KEY_KP_2
        #define ENGINE_KEY_KP_3 GLFW_KEY_KP_3
        #define ENGINE_KEY_KP_4 GLFW_KEY_KP_4
        #define ENGINE_KEY_KP_5 GLFW_KEY_KP_5
        #define ENGINE_KEY_KP_6 GLFW_KEY_KP_6
        #define ENGINE_KEY_KP_7 GLFW_KEY_KP_7
        #define ENGINE_KEY_KP_8 GLFW_KEY_KP_8
        #define ENGINE_KEY_KP_9 GLFW_KEY_KP_9
        #define ENGINE_KEY_KP_DECIMAL GLFW_KEY_KP_DECIMAL
        #define ENGINE_KEY_KP_DIVIDE GLFW_KEY_KP_DIVIDE
        #define ENGINE_KEY_KP_MULTIPLY GLFW_KEY_KP_MULTIPLY
        #define ENGINE_KEY_KP_SUBTRACT GLFW_KEY_KP_SUBTRACT
        #define ENGINE_KEY_KP_ADD GLFW_KEY_KP_ADD
        #define ENGINE_KEY_KP_ENTER GLFW_KEY_KP_ENTER
        #define ENGINE_KEY_KP_EQUAL GLFW_KEY_KP_EQUAL
        #define ENGINE_KEY_LEFT_SHIFT GLFW_KEY_LEFT_SHIFT
        #define ENGINE_KEY_LEFT_CONTROL GLFW_KEY_LEFT_CONTROL
        #define ENGINE_KEY_LEFT_ALT GLFW_KEY_LEFT_ALT
        #define ENGINE_KEY_LEFT_SUPER GLFW_KEY_LEFT_SUPER
        #define ENGINE_KEY_RIGHT_SHIFT GLFW_KEY_RIGHT_SHIFT
        #define ENGINE_KEY_RIGHT_CONTROL GLFW_KEY_RIGHT_CONTROL
        #define ENGINE_KEY_RIGHT_ALT GLFW_KEY_RIGHT_ALT
        #define ENGINE_KEY_RIGHT_SUPER GLFW_KEY_RIGHT_SUPER
        #define ENGINE_KEY_MENU GLFW_KEY_MENU
        #define ENGINE_KEY_MENU GLFW_KEY_MENU

    #endif

    #ifdef ENGINE_USE_QT_WIDGET

enum ENGINE_KEY {
        ENGINE_KEY_ESCAPE = 0X01000000,                // MISC ENGINE_KEYS
        ENGINE_KEY_TAB = 0X01000001,
        ENGINE_KEY_BACKTAB = 0X01000002,
        ENGINE_KEY_BACKSPACE = 0X01000003,
        ENGINE_KEY_RETURN = 0X01000004,
        ENGINE_KEY_ENTER = 0X01000005,
        ENGINE_KEY_INSERT = 0X01000006,
        ENGINE_KEY_DELETE = 0X01000007,
        ENGINE_KEY_PAUSE = 0X01000008,
        ENGINE_KEY_PRINT = 0X01000009,
        ENGINE_KEY_SYSREQ = 0X0100000A,
        ENGINE_KEY_CLEAR = 0X0100000B,
        ENGINE_KEY_HOME = 0X01000010,                // CURSOR MOVEMENT
        ENGINE_KEY_END = 0X01000011,
        ENGINE_KEY_LEFT = 0X01000012,
        ENGINE_KEY_UP = 0X01000013,
        ENGINE_KEY_RIGHT = 0X01000014,
        ENGINE_KEY_DOWN = 0X01000015,
        ENGINE_KEY_PAGEUP = 0X01000016,
        ENGINE_KEY_PAGEDOWN = 0X01000017,
        ENGINE_KEY_SHIFT = 0X01000020,                // MODIFIERS
        ENGINE_KEY_LEFT_SHIFT = ENGINE_KEY_SHIFT,
        ENGINE_KEY_CONTROL = 0X01000021,
        ENGINE_KEY_META = 0X01000022,
        ENGINE_KEY_ALT = 0X01000023,
        ENGINE_KEY_CAPSLOCK = 0X01000024,
        ENGINE_KEY_NUMLOCK = 0X01000025,
        ENGINE_KEY_SCROLLLOCK = 0X01000026,
        ENGINE_KEY_F1 = 0X01000030,                // FUNCTION ENGINE_KEYS
        ENGINE_KEY_F2 = 0X01000031,
        ENGINE_KEY_F3 = 0X01000032,
        ENGINE_KEY_F4 = 0X01000033,
        ENGINE_KEY_F5 = 0X01000034,
        ENGINE_KEY_F6 = 0X01000035,
        ENGINE_KEY_F7 = 0X01000036,
        ENGINE_KEY_F8 = 0X01000037,
        ENGINE_KEY_F9 = 0X01000038,
        ENGINE_KEY_F10 = 0X01000039,
        ENGINE_KEY_F11 = 0X0100003A,
        ENGINE_KEY_F12 = 0X0100003B,
        ENGINE_KEY_F13 = 0X0100003C,
        ENGINE_KEY_F14 = 0X0100003D,
        ENGINE_KEY_F15 = 0X0100003E,
        ENGINE_KEY_F16 = 0X0100003F,
        ENGINE_KEY_F17 = 0X01000040,
        ENGINE_KEY_F18 = 0X01000041,
        ENGINE_KEY_F19 = 0X01000042,
        ENGINE_KEY_F20 = 0X01000043,
        ENGINE_KEY_F21 = 0X01000044,
        ENGINE_KEY_F22 = 0X01000045,
        ENGINE_KEY_F23 = 0X01000046,
        ENGINE_KEY_F24 = 0X01000047,
        ENGINE_KEY_F25 = 0X01000048,                // F25 .. F35 ONLY ON X11
        ENGINE_KEY_F26 = 0X01000049,
        ENGINE_KEY_F27 = 0X0100004A,
        ENGINE_KEY_F28 = 0X0100004B,
        ENGINE_KEY_F29 = 0X0100004C,
        ENGINE_KEY_F30 = 0X0100004D,
        ENGINE_KEY_F31 = 0X0100004E,
        ENGINE_KEY_F32 = 0X0100004F,
        ENGINE_KEY_F33 = 0X01000050,
        ENGINE_KEY_F34 = 0X01000051,
        ENGINE_KEY_F35 = 0X01000052,
        ENGINE_KEY_SUPER_L = 0X01000053,                 // EXTRA ENGINE_KEYS
        ENGINE_KEY_SUPER_R = 0X01000054,
        ENGINE_KEY_MENU = 0X01000055,
        ENGINE_KEY_HYPER_L = 0X01000056,
        ENGINE_KEY_HYPER_R = 0X01000057,
        ENGINE_KEY_HELP = 0X01000058,
        ENGINE_KEY_DIRECTION_L = 0X01000059,
        ENGINE_KEY_DIRECTION_R = 0X01000060,
        ENGINE_KEY_SPACE = 0X20,                // 7 BIT PRINTABLE ASCII
        ENGINE_KEY_ANY = ENGINE_KEY_SPACE,
        ENGINE_KEY_EXCLAM = 0X21,
        ENGINE_KEY_QUOTEDBL = 0X22,
        ENGINE_KEY_NUMBERSIGN = 0X23,
        ENGINE_KEY_DOLLAR = 0X24,
        ENGINE_KEY_PERCENT = 0X25,
        ENGINE_KEY_AMPERSAND = 0X26,
        ENGINE_KEY_APOSTROPHE = 0X27,
        ENGINE_KEY_PARENLEFT = 0X28,
        ENGINE_KEY_PARENRIGHT = 0X29,
        ENGINE_KEY_ASTERISK = 0X2A,
        ENGINE_KEY_PLUS = 0X2B,
        ENGINE_KEY_COMMA = 0X2C,
        ENGINE_KEY_MINUS = 0X2D,
        ENGINE_KEY_PERIOD = 0X2E,
        ENGINE_KEY_SLASH = 0X2F,
        ENGINE_KEY_0 = 0X30,
        ENGINE_KEY_1 = 0X31,
        ENGINE_KEY_2 = 0X32,
        ENGINE_KEY_3 = 0X33,
        ENGINE_KEY_4 = 0X34,
        ENGINE_KEY_5 = 0X35,
        ENGINE_KEY_6 = 0X36,
        ENGINE_KEY_7 = 0X37,
        ENGINE_KEY_8 = 0X38,
        ENGINE_KEY_9 = 0X39,
        ENGINE_KEY_COLON = 0X3A,
        ENGINE_KEY_SEMICOLON = 0X3B,
        ENGINE_KEY_LESS = 0X3C,
        ENGINE_KEY_EQUAL = 0X3D,
        ENGINE_KEY_GREATER = 0X3E,
        ENGINE_KEY_QUESTION = 0X3F,
        ENGINE_KEY_AT = 0X40,
        ENGINE_KEY_A = 0X41,
        ENGINE_KEY_B = 0X42,
        ENGINE_KEY_C = 0X43,
        ENGINE_KEY_D = 0X44,
        ENGINE_KEY_E = 0X45,
        ENGINE_KEY_F = 0X46,
        ENGINE_KEY_G = 0X47,
        ENGINE_KEY_H = 0X48,
        ENGINE_KEY_I = 0X49,
        ENGINE_KEY_J = 0X4A,
        ENGINE_KEY_K = 0X4B,
        ENGINE_KEY_L = 0X4C,
        ENGINE_KEY_M = 0X4D,
        ENGINE_KEY_N = 0X4E,
        ENGINE_KEY_O = 0X4F,
        ENGINE_KEY_P = 0X50,
        ENGINE_KEY_Q = 0X51,
        ENGINE_KEY_R = 0X52,
        ENGINE_KEY_S = 0X53,
        ENGINE_KEY_T = 0X54,
        ENGINE_KEY_U = 0X55,
        ENGINE_KEY_V = 0X56,
        ENGINE_KEY_W = 0X57,
        ENGINE_KEY_X = 0X58,
        ENGINE_KEY_Y = 0X59,
        ENGINE_KEY_Z = 0X5A

};

    #endif



	struct MouseState
	{
		enum Button {
            LEFT_BUTTON = 1,
            MIDDLE_BUTTON = 2,
            RIGHT_BUTTON = 3
		};

		float x, y;
		bool button[3], isMoving;
		int wheelDelta;

		MouseState() {
			x = y = 0;
			button[0] = 0;
			button[1] = 0;
			button[2] = 0;
			isMoving = 0;
			wheelDelta = 0;
		}
	};

	struct JoystickState
	{
		bool button[10];
		glm::vec2 rightAnalog, leftAnalog, POV;
		float triggerLeft, triggerRight;
		JoystickState() {
			for (int i = 0; i < 10; i++)button[i] = 0;
			triggerLeft = triggerRight = 0;
		}
	};

	struct InputState
	{
		bool key[350];
		MouseState mouse;
		JoystickState joystick[4];

		InputState() {
			for (int i = 0; i < 350; i++)key[i] = 0;
		}
	};

}
