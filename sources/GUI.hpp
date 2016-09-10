#ifndef GUI_hpp
#define GUI_hpp

#include <string>
#include "RFGL.hpp"
#include "IRunable.hpp"
#include <AntTweakBar.h>

//struct TwBar;

class GUI
{
public:
	GUI() = delete;

	static void init();

	static void addBool(const std::string& label,  bool& value);

	static void addDouble(const std::string& label, double& value);

	static void update();

	static void clean();
    
    static void onKey(int key, int action)
    {
        TwEventKeyGLFW(key, action);
    }
    static void onMouseButton(int button, int action)
    {
        TwEventMouseButtonGLFW(button, action);
    }
    static void onMouseScroll(double yoffset)
    {
        TwEventMouseWheelGLFW(yoffset);
    }

private:
	static TwBar *m_twBar;
};

#endif // GUI_hpp