#include "Transform.hpp"
#include "GameObject.hpp"
//const std::string Transform::className = "Transform";

Transform::Transform() 
	: m_localPosition(0, 0, 0), m_localScale(1, 1, 1), m_localRotation(1, 0, 0, 0)
{
}
