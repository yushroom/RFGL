#ifndef Component_hpp
#define Component_hpp

#include "RFGL.hpp"

class GameObject;

//template<typename T>
//static std::string getClassName(std::shared_ptr<T> v) {
//	return T::getClassName();
//}

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
	GameObject* gameObject;
};

#endif // Component_hpp