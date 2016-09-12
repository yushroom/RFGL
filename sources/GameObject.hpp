#ifndef GameObject_hpp
#define GameObject_hpp

#include "Transform.hpp"
#include "Behavior.hpp"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void addComponent(std::shared_ptr<Component> component) {
		component->m_gameObject = this;
		component->m_transform = &m_transform;
		if (component->m_transform->parent() == nullptr) {
			component->m_transform->setParent(&m_root.m_transform);
		}
		m_components.push_back(component);
	}
    
    void addScript(std::shared_ptr<Script> script) {
        script->m_gameObject = this;
		script->m_transform = &m_transform;
		if (script->m_transform->parent() == nullptr) {
			script->m_transform->setParent(&m_root.m_transform);
		}
        m_scripts.push_back(script);
    }

	template<typename T>
	std::shared_ptr<T> getComponent() {
		for (auto& comp : m_components) {
			if (comp->getClassName() == T::className()) {
				return std::static_pointer_cast<T>(comp);
			}
		}
		return nullptr;
	}
    
	void start() {
		m_transform.start();
//        for (auto& c : m_components) {
//            c->init();
//        }
		for (auto& s : m_scripts) {
			s->start();
		}
	}

    void update() {
        m_transform.update();
        //for (auto& s : )
        for (auto& s : m_scripts){
            s->update();
        }
    }
    
    Transform* transform() {
        return &m_transform;
    }
    
    const Transform* transform() const {
        return &m_transform;
    }

private:
	std::vector<std::shared_ptr<Component>> m_components;
    std::vector<std::shared_ptr<Script>> m_scripts;
    
    Transform m_transform;
    
    static GameObject m_root;
};

#endif // GameObject_hpp
