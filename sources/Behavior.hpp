#ifndef Behaviour_hpp
#define Behaviour_hpp

#include "Component.hpp"

class Behaviour : public Component
{

};

class Script : public Behaviour
{
public:
    ClassName(Script)
    //virtual onEnable();
    virtual void start() {};
    virtual void fixedUpdate() {};
    virtual void update() = 0;
    virtual void lateUpdate() {};
    virtual void onGUI() {};
    
};


#endif // Behaviour_hpp