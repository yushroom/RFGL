#ifndef App_hpp
#define App_hpp

#include "RenderSystem.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "GLError.hpp"
#include "Camera.hpp"

class App : public IRunable
{
public:
    virtual ~App() = 0 {}
    //virtual void OnMouse() {};
    //virtual void OnKey() {};
};

#endif // App_hpp