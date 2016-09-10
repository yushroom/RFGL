#include "Input.hpp"
#include <cassert>

Input::KeyState Input::m_keyStates[1024] = { Input::KeyState_None };
Input::MouseButtonState Input::m_mouseButtonStates[3] = {Input::MouseButtonState_None};

float Input::m_mousePositionX = 0;
float Input::m_mousePositionY = 0;
float Input::m_axis[4] = {0, 0, 0, 0};

void Input::init()
{
	for (auto& s : m_keyStates)
		s = KeyState_None;
	for (auto& b : m_mouseButtonStates)
		b = MouseButtonState_None;
}

bool Input::getKey(KeyCode key)
{
	return m_keyStates[key] == KeyState_Held;
}

bool Input::getKeyDown(KeyCode key)
{
	return m_keyStates[key] == KeyState_Down;
}

bool Input::getKeyUp(KeyCode key)
{
	return m_keyStates[key] == KeyState_Up;
}

bool Input::getMouseButton(int button)
{
	if (button >= 0 && button <= 3) {
		return m_mouseButtonStates[button] == MouseButtonState_Held;
	}
	else {
		warning("invalid mouse button id: %d", button);
		return false;
	}
}

bool Input::getMouseButtonDown(int button)
{
	if (button >= 0 && button <= 3) {
		return m_mouseButtonStates[button] == MouseButtonState_Down;
	}
	else {
		warning("invalid mouse button id: %d", button);
		return false;
	}
}

bool Input::getMouseButtonUp(int button)
{
	if (button >= 0 && button <= 3) {
		return m_mouseButtonStates[button] == MouseButtonState_Up;
	}
	else {
		warning("invalid mouse button id: %d", button);
		return false;
	}
}

void Input::update()
{
	//memset(m_keyStates, 0, 1024 * sizeof(KeyState));
	for (auto& s : m_keyStates) {
		if (s == KeyState_Up || s == KeyState_Down)
			s = KeyState_None;
	}

	for (auto& b : m_mouseButtonStates) {
		if (b == MouseButtonState_Down) b = MouseButtonState_Held;
		else if (b == MouseButtonState_Up) b = MouseButtonState_None;
	}
}

void Input::updateMousePosition(float xpos, float ypos)
{
	m_axis[Axis_MouseX] = xpos - m_mousePositionX;
	m_axis[Axis_MouseY] = -(ypos - m_mousePositionY);
	m_mousePositionX = xpos;
	m_mousePositionY = ypos;
}

void Input::updateKeyState(KeyCode key, KeyState state)
{
	//info("%d %d", key, state);
	m_keyStates[key] = state;
}

void Input::updateMouseButtonState(int button, MouseButtonState state)
{
	assert(button >= 0 && button <= 3);
	m_mouseButtonStates[button] = state;
}
