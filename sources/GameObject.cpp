#include "GameObject.hpp"
#include "Scene.hpp"

GameObject GameObject::m_root;

GameObject::PGameObject GameObject::Find(const std::string& name)
{
    return Scene::Find(name);
}
