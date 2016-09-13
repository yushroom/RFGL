#include "GUI.hpp"

#include <sstream>

#include "RenderSystem.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"


void GUI::Init()
{

}

void GUI::Update()
{
    ImGui::Render();
}

void GUI::Clean()
{
    ImGui_ImplGlfwGL3_Shutdown();
}

//void GUI::OnKey(int key, int action)
//{
//}
//
//void GUI::OnWindowSizeChanged(int width, int height)
//{
//}
//
//void GUI::OnMouse(double xpos, double ypos)
//{
//}
//
//bool GUI::OnMouseButton(int button, int action)
//{
//    return false;
//}
//
//bool GUI::OnMouseScroll(double yoffset)
//{
//    return false;
//}
