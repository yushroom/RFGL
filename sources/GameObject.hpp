#ifndef GameObject_hpp
#define GameObject_hpp

#include "Transform.hpp"

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

	template<typename T>
	std::shared_ptr<T> getComponent() {
		for (auto& comp : m_components) {
			if (comp->getClassName() == T::className()) {
				return std::static_pointer_cast<T>(comp);
			}
		}
		return nullptr;
	}

private:
	std::vector<std::shared_ptr<Component>> m_components;
};

#endif // GameObject_hpp
