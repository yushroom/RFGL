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

	static void Init();

	static void AddBool(const std::string& label,  bool& value);
	static void AddFloat(const std::string& label, const float& value);
	static void AddVector3(const std::string& label, const Vector3& value);
	static void AddDouble(const std::string& label, const double& value);

	static void Update();

	static void Clean();
    
    static void OnKey(int key, int action);
	static void OnWindowSizeChanged(int width, int height);
	static void OnMouse(double xpos, double ypos);
    static bool OnMouseButton(int button, int action);

	// return true if the mouse wheel event has been handled by GUI
    static bool OnMouseScroll(double yoffset);

private:
	static TwBar *m_twBar;
};

#endif // GUI_hpp