#include "GameObject.hpp"

GameObject GameObject::m_root;

GameObject::GameObject()
{
    m_transform.m_gameObject = this;
}


GameObject::~GameObject()
{
}
