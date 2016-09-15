#include "GUI.hpp"

//#include <sstream>

//#include "RenderSystem.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "GameObject.hpp"
#include "Scene.hpp"

int GUI::m_idCount = 0;

void GUI::Init()
{

}

void GUI::Update()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New Scene", "Ctrl+N");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    ImGui::Begin("Hierarchy");
    ImGui::Button("Create");
    m_idCount = 0;
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize()); // Increase spacing to differentiate leaves from expanded contents.
    for (auto go : Scene::m_gameObjects) {
        if (go->transform()->parent() == nullptr) {
            HierarchyItem(go.get());
        }
    }
    ImGui::PopStyleVar();
    ImGui::End();
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

void GUI::HierarchyItem(GameObject* gameObject) {
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((gameObject == Scene::m_activeGameObject)? ImGuiTreeNodeFlags_Selected : 0);
    
    bool is_leaf = (gameObject->transform()->childCount() == 0);
    if (is_leaf) {// no child
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)m_idCount, node_flags, "%s", gameObject->name().c_str());
    if (ImGui::IsItemClicked()) {
        Scene::SelectGameObject(gameObject);
    }
    // child node
    if (!is_leaf) {
        if (node_open) {
            for (auto t : gameObject->transform()->m_children)
                HierarchyItem(t->gameObject());
            ImGui::TreePop();
        }
    }
    m_idCount ++;
}
