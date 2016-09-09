#ifndef Input_hpp
#define Input_hpp

#include "RFGL.hpp"
#include "Log.hpp"

class RenderSystem;

class Input {
public:

	friend class RenderSystem;

	enum KeyCode {
		KeyCode_0 = GLFW_KEY_0, KeyCode_1, KeyCode_2, KeyCode_3, KeyCode_4, KeyCode_5, KeyCode_6, KeyCode_7, KeyCode_8, KeyCode_9, KeyCode_A = GLFW_KEY_A, KeyCode_B, KeyCode_C, KeyCode_D, KeyCode_E, KeyCode_F, KeyCode_G, KeyCode_H, KeyCode_I, KeyCode_J, KeyCode_K, KeyCode_L, KeyCode_M, KeyCode_N, KeyCode_O, KeyCode_P, KeyCode_Q, KeyCode_R, KeyCode_S, KeyCode_T, KeyCode_U, KeyCode_V, KeyCode_W, KeyCode_X, KeyCode_Y, KeyCode_Z
	};
	enum KeyState {
		KeyState_Up   = GLFW_RELEASE, // 0
		KeyState_Down = GLFW_PRESS,  // 1
		KeyState_Held = GLFW_REPEAT, // 2
		KeyState_None = 3,
	};

	Input() = delete;

	// Returns the value of the virtual axis identified by axisName.
	static void getAxis();

	// Returns true while the user holds down the key identified by name. Think auto fire.
	static bool getKey(KeyCode key);

	// Returns true during the frame the user starts pressing down the key identified by name.
	static bool getKeyDown(KeyCode key);

	// Returns true during the frame the user releases the key identified by name.
	static bool getKeyUp(KeyCode key);

	// Returns whether the given mouse button is held down.
	static bool getMouseButton();

	// Returns true during the frame the user pressed the given mouse button.
	static bool getMouseButtonDown();

	// Returns true during the frame the user releases the given mouse button.
	static bool getMouseButtonUp();

private:
	static KeyState m_keyStates[1024];

	static void init();
	static void reset();
	static void updateKeyState(KeyCode key, KeyState state);
};

#endif // Input_hpp