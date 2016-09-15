#ifndef App_hpp
#define App_hpp

#include "IRunable.hpp"

class App : public IRunable
{
public:
    virtual ~App() = 0;
    //virtual void OnMouse() {};
    //virtual void OnKey() {};
};



#endif // App_hpp