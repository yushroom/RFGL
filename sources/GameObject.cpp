#include "GameObject.hpp"
#include "Scene.hpp"

GameObject GameObject::m_root("Root");

GameObject::PGameObject GameObject::Find(const std::string& name)
{
    return Scene::Find(name);
}
