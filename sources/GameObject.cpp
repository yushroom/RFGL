#include "GameObject.hpp"
#include "Scene.hpp"

GameObject GameObject::m_root;

GameObject::GameObject()
{
    m_transform.m_gameObject = this;
}


GameObject::~GameObject()
{
}

GameObject::PGameObject GameObject::Find(const std::string& name)
{
    return Scene::Find(name);
}
