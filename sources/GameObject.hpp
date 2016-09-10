#ifndef GameObject_hpp
#define GameObject_hpp

#include "Transform.hpp"
#include "Behavior.hpp"

class GameObject
{
public:
	Transform transform;

public:
	GameObject();
	~GameObject();

	void addComponent(std::shared_ptr<Component> component) {
		component->gameObject = this;
		m_components.push_back(component);
	}
    
    void addScript(std::shared_ptr<Script> script) {
        script->gameObject = this;
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
    
    void update() {
        transform.update();
        for (auto& comp : m_scripts){
            comp->update();
        }
    }

private:
	std::vector<std::shared_ptr<Component>> m_components;
    std::vector<std::shared_ptr<Script>> m_scripts;
};

#endif // GameObject_hpp
