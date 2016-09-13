#ifndef Scene_hpp
#define Scene_hpp

#include "GameObject.hpp"
#include "Camera.hpp"

class Scene
{
public:
    static std::shared_ptr<Camera> mainCamera() {
        return m_mainCamera;
    }
    
    static std::shared_ptr<GameObject> CreateGameObject() {
        auto go = std::make_shared<GameObject>();
        m_gameObjects.push_back(go);
        return go;
    }
    
    static void Init();
    static void Start();
    static void Update();
    static void Render();

    static GameObject::PGameObject Find(const std::string& name) {
        for (auto& go : m_gameObjects) {
            if (go->m_name == name) {
                return go;
            }
        }
        return nullptr;
    }
    
private:
    static std::shared_ptr<Camera> m_mainCamera;
    static std::vector<std::shared_ptr<GameObject>> m_gameObjects;
    
};

#endif /* Scene_hpp */
