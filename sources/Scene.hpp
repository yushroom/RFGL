#ifndef Scene_hpp
#define Scene_hpp

#include "GameObject.hpp"
#include "Camera.hpp"

class RenderSystem;

class Scene
{
public:
    static std::shared_ptr<Camera> mainCamera() {
        return m_mainCamera;
    }
    
    static std::shared_ptr<GameObject> CreateGameObject(const std::string& name) {
        auto go = std::make_shared<GameObject>(name);
        m_gameObjects.push_back(go);
        return go;
    }
    
    static void Init();
    static void Start();
    static void Update();
    static void Render();
    static void OnEditorGUI();

    static void SelectGameObject(GameObject* gameObject) {
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
    friend class RenderSystem;
    friend class GUI;
    static std::shared_ptr<Camera> m_mainCamera;
    static std::vector<std::shared_ptr<GameObject>> m_gameObjects;
    
    // the selected GameObject in the editor
    //static std::shared_ptr<GameObject> m_activeGameObject;
    static GameObject* m_activeGameObject;
};

#endif /* Scene_hpp */
