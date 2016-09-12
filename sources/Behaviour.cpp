#include "Behaviour.hpp"
#include "GameObject.hpp"


bool Behaviour::isActiveAndEnabled() const
{
    return  m_enabled && m_gameObject->activeInHierarchy();
}
