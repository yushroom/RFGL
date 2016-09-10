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
	static void addFloat(const std::string& label, const float& value);
	static void addVector3(const std::string& label, const Vector3& value);
	static void addDouble(const std::string& label, const double& value);

	static void update();

	static void clean();
    
    static void onKey(int key, int action);
	static void onWindowSizeChanged(int width, int height);
	static void onMouse(double xpos, double ypos);
    static bool onMouseButton(int button, int action);

	// return true if the mouse wheel event has been handled by GUI
    static bool onMouseScroll(double yoffset);

private:
	static TwBar *m_twBar;
};

#endif // GUI_hpp