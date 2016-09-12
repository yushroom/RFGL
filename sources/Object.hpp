#ifndef Object_hpp
#define Object_hpp

#include "RFGL.hpp"

//#define Property(T, var) \
//	protected:\
//		T m_##var;\
//	public:\
//		T var () const { return m_##var; }\
//		void set##var(const T & var) { m_##var = var; }

class Object
{
public:
    // The name of the object.
    std::string name() const {return m_name; }
    void setName(const std::string& name) { m_name = name; }

    virtual ~Object() = 0;

protected:
    std::string m_name;
};

#endif // Object_hpp