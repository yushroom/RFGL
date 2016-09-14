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
    static void OnEditorGUI();

    static void SelectGameObject(std::shared_ptr<GameObject> gameObject) {
        m_activeGameObject = gameObject;
    }
    
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
    
    // the selected GameObject in the editor
    static std::shared_ptr<GameObject> m_activeGameObject;
};

#endif /* Scene_hpp */
