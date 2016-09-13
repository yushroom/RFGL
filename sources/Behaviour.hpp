#ifndef Behaviour_hpp
#define Behaviour_hpp

#include "Component.hpp"

// Behaviours are Components that can be enabled or disabled.
class Behaviour : public Component
{
public:

    // Enabled Behaviours are Updated, disabled Behaviours are not.
    bool enabled() const { return m_enabled; }
    void setEnabled(bool value) { m_enabled = value; }

    // Has the Behaviour had enabled called.
    bool isActiveAndEnabled() const;

private:
    bool m_enabled = true;
};


// Script is the base class every script derives from.
// http://docs.unity3d.com/ScriptReference/MonoBehaviour.html
// http://docs.unity3d.com/Manual/ExecutionOrder.html
class Script : public Behaviour
{
public:
    InjectClassName(Script);
    //virtual onEnable();

    // Awake is called when the script instance is being loaded.
    virtual void Awake() {}

    virtual void Start() {};
    
    virtual void FixedUpdate() {};
    
    virtual void Update() = 0;
    
    virtual void LateUpdate() {};
    
    virtual void OnGUI() {};
};


#endif // Behaviour_hpp