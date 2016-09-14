#ifndef GUI_hpp
#define GUI_hpp

#include <string>
#include "RFGL.hpp"
#include "IRunable.hpp"
#include "imgui/imgui.h"

class GUI
{
public:
    GUI() = delete;

    static void Init();

    static void Update();

    static void Clean();
    
    //static void OnKey(int key, int action);
    //static void OnWindowSizeChanged(int width, int height);
    //static void OnMouse(double xpos, double ypos);
    //static bool OnMouseButton(int button, int action);

    // return true if the mouse wheel event has been handled by GUI
    //static bool OnMouseScroll(double yoffset);
    
    // Make a single press button. The user clicks them and something happens immediately.
    //static void bool Button(Re)

    //typedef void(*SetterCallback)(void);

    //static void Vector3(const char* label, Vector3& v, SetterCallback) {
    //    ImGui::InputFloat3(label, glm::value_ptr(v));
    //}
};

#endif // GUI_hpp