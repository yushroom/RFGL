#ifndef Input_hpp
#define Input_hpp

#include "RFGL.hpp"

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

	enum MouseButtonCode {
		MouseButtonCode_Left = 0,
		MouseButtonCode_Right = 1,
		MouseButtonCode_Middle = 2,
	};

	enum MouseButtonState {
		MouseButtonState_None = 0,
		MouseButtonState_Down = 1,
		MouseButtonState_Held = 2,
		MouseButtonState_Up = 3,
	};

	enum Axis {
		Axis_Vertical = 0,
		Axis_Horizontal,
		Axis_MouseX,
		Axis_MouseY,
	};

	Input() = delete;

	// Returns the value of the virtual axis identified by axisName.
	static float GetAxis(Axis axis) {
		return m_axis[axis];
	}

	// Returns true while the user holds down the key identified by name. Think auto fire.
	static bool GetKey(KeyCode key);

	// Returns true during the frame the user starts pressing down the key identified by name.
	static bool GetKeyDown(KeyCode key);

	// Returns true during the frame the user releases the key identified by name.
	static bool GetKeyUp(KeyCode key);

	// Returns whether the given mouse button is held down.
	// button values are 0 for left button, 1 for right button, 2 for the middle button.
	static bool GetMouseButton(int button);

	// Returns true during the frame the user pressed the given mouse button.
	// button values are 0 for left button, 1 for right button, 2 for the middle button.
	static bool GetMouseButtonDown(int button);

	// Returns true during the frame the user releases the given mouse button.
	// button values are 0 for left button, 1 for right button, 2 for the middle button.
	static bool GetMouseButtonUp(int button);

private:
	static KeyState m_keyStates[1024];

	// button values are 0 for left button, 1 for right button, 2 for the middle button.
	static MouseButtonState m_mouseButtonStates[3];
	static float m_mousePositionX;
	static float m_mousePositionY;
	static float m_axis[4];

	static void Init();
	static void Update();

	static void UpdateMousePosition(float xpos, float ypos);
	static void UpdateKeyState(KeyCode key, KeyState state);
	static void UpdateMouseButtonState(int button, MouseButtonState state);
};

#endif // Input_hpp