#ifndef Component_hpp
#define Component_hpp

#include "RFGL.hpp"
#include "Object.hpp"

class GameObject;
class Transform;

// Base class for everything attached to GameObjects.
class Component : public Object
{
public:
	virtual std::string ClassName() const = 0;

    // The game object this component is attached to. A component is always attached to a game object.
    GameObject* gameObject() const { return m_gameObject; }
    
    // The tag of this game object.
    std::string tag() const;
    
    // The Transform attached to this GameObject (null if there is none attached).
    Transform* transform() const;
    
    friend class GameObject;
    
protected:
	GameObject* m_gameObject = nullptr;
};


#define InjectClassName(name) \
	static std::string StaticClassName() { \
		return #name; \
	} \
	virtual std::string ClassName() const override { \
		return StaticClassName(); \
	}

#endif // Component_hpp