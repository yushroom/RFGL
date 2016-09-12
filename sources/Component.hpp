#ifndef Component_hpp
#define Component_hpp

#include "RFGL.hpp"

class GameObject;
class Transform;

#define ClassName(name) \
	static std::string className() { \
		return #name; \
	} \
	virtual std::string getClassName() const override { \
		return className(); \
	}

class Component
{
public:
	virtual std::string getClassName() const = 0;
    
    GameObject* gameObject() const {
        return m_gameObject;
    }
    
    Transform* transform() const {
        return m_transform;
    }
    
    friend class GameObject;
    
protected:
	GameObject* m_gameObject;
	Transform* m_transform;
};

#endif // Component_hpp