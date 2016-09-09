#include "Input.hpp"

Input::KeyState Input::m_keyStates[1024] = { Input::KeyState_None };

void Input::init()
{
	for (auto& s : m_keyStates)
		s = KeyState_None;
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

void Input::reset()
{
	//memset(m_keyStates, 0, 1024 * sizeof(KeyState));
	for (auto& s : m_keyStates) {
		if (s == KeyState_Up || s == KeyState_Down)
			s = KeyState_None;
	}
}

void Input::updateKeyState(KeyCode key, KeyState state)
{
	//info("%d %d", key, state);
	m_keyStates[key] = state;
}
