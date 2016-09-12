#include "Component.hpp"
#include "GameObject.hpp"

std::string Component::tag() const
{
    if (m_gameObject == nullptr)
        return "";
    return m_gameObject->tag();
}

Transform* Component::transform() const
{
    if (m_gameObject == nullptr)
        return nullptr;
    return m_gameObject->transform();
}
