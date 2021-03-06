#ifndef GameObject_hpp
#define GameObject_hpp

#include "Transform.hpp"
#include "Behaviour.hpp"
#include "Object.hpp"
#include "Common.hpp"

class Scene;

// Base class for all entities in Unity scenes.
// http://docs.unity3d.com/ScriptReference/GameObject.html
class GameObject : public Object
{
public:
//private:
    GameObject(const std::string& name) : m_tag("Untagged") {
        m_transform.m_gameObject = this;
        m_name = name;
    }
    
public:
    ~GameObject() = default;

    typedef std::shared_ptr<GameObject> PGameObject;

    //========== Variables ==========//

    // Is the GameObject active in the scene ?
    bool activeInHierarchy() const {
        if (m_activeSelf && m_transform.parent() != nullptr) {
            return m_transform.parent()->gameObject()->activeInHierarchy();
        }
        return m_activeSelf;
    }

    // The local active state of this GameObject. (Read Only)
    bool activeSelf() const {
        return m_activeSelf;
    }

    // The layer the game object is in. A layer is in the range [0...31].
    int layer() const {
        return m_layer;
    }

    void SetLayer(int layer) {
        m_layer = layer;
    }

    // The tag of this game object.
    std::string tag() const {
        return m_tag;
    }

    void SetTag(const std::string& tag) {
        m_tag = tag;
    }

    // The Transform attached to this GameObject.
    Transform* transform() {
        return &m_transform;
    }

    //========== Public Functions ==========//

    // Returns the component of Type type if the game object has one attached, null if it doesn't.
    template<typename T>
    std::shared_ptr<T> GetComponent() {
        for (auto& comp : m_components) {
            if (comp->ClassName() == T::StaticClassName()) {
                return std::static_pointer_cast<T>(comp);
            }
        }
        return nullptr;
    }

    // Adds a component class named className to the game object.
    void AddComponent(std::shared_ptr<Component> component) {
        component->m_gameObject = this;
        m_components.push_back(component);
    }
    
    void AddScript(std::shared_ptr<Script> script) {
        script->m_gameObject = this;
        m_scripts.push_back(script);
    }

    // Activates/Deactivates the GameObject (activeSelf).
    void SetActive(bool value) {
        m_activeSelf = value;
    }

    //========== Static Functions ==========//

    // Finds a game object by name and returns it.
    static PGameObject Find(const std::string& name);


protected:
    friend class Scene;
    void Start() {
        //m_transform.Start();
//        for (auto& c : m_components) {
//            c->init();
//        }
        for (auto& s : m_scripts) {
            s->Start();
        }
    }

    void Update() {
        m_transform.Update();
        //for (auto& s : )
        for (auto& s : m_scripts){
            s->Update();
        }
    }

    void OnEditorGUI() {
        ImGui::PushID("Inspector.selected.active");
        ImGui::Checkbox("", &m_activeSelf);
        ImGui::PopID();
        char name[32] = {0};
        memcpy(name, m_name.c_str(), m_name.size());
        name[m_name.size()] = 0;
        ImGui::SameLine();
        ImGui::PushID("Inspector.selected.name");
        if (ImGui::InputText("", name, 31)) {
            m_name = name;
        }
        ImGui::PopID();
        
        ImGui::LabelText("Tag", "%s", m_tag.c_str());
        //ImGui::SameLine();
        ImGui::LabelText("Layer", "Layer %d", m_layer);
        
        if (ImGui::CollapsingHeader("Transform")) {
            m_transform.OnEditorGUI();
        }
        
        for (auto& c : m_components) {
            if (ImGui::CollapsingHeader(camelCaseToReadable(c->ClassName()).c_str()))
                c->OnEditorGUI();
        }
        for (auto& s : m_scripts) {
            if (ImGui::CollapsingHeader(camelCaseToReadable(s->ClassName()).c_str()))
                s->OnEditorGUI();
        }
    }
    
    const Transform* transform() const {
        return &m_transform;
    }

private:
    std::vector<std::shared_ptr<Component>> m_components;
    std::vector<std::shared_ptr<Script>> m_scripts;
    
    bool m_activeSelf = true;
    int m_layer = 0;

    std::string m_tag;
    Transform m_transform;
    
    static GameObject m_root;
};

#endif // GameObject_hpp
