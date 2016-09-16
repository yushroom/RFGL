#include "EditorGUI.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "GameObject.hpp"
#include "Scene.hpp"
#include "Material.hpp"
#include "MeshFilter.hpp"
#include "MeshRenderer.hpp"

int EditorGUI::m_idCount = 0;

Material::PMaterial axisIndicatorMaterial;
Mesh::PMesh cubeMesh;
Mesh::PMesh coneMesh;

void EditorGUI::Init()
{
#if defined(_WIN32)
    const std::string root_dir = "../";
#else
    const std::string root_dir = "/Users/yushroom/program/graphics/RFGL/";
#endif
    const std::string models_dir = root_dir + "models/";
 
    axisIndicatorMaterial = Material::builtinMaterial("VertexLit");
    cubeMesh = Mesh::CreateFromObjFile(models_dir+"box.obj");
    coneMesh = Mesh::CreateFromObjFile(models_dir+"cone.obj");
}

void EditorGUI::Update()
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
    
    DrawAxisIndicator();
}

void EditorGUI::Clean()
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

void EditorGUI::HierarchyItem(GameObject* gameObject) {
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((gameObject == Scene::m_activeGameObject)? ImGuiTreeNodeFlags_Selected : 0);
    
    bool is_leaf = (gameObject->transform()->childCount() == 0);
    if (is_leaf) {// no child
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    if (!gameObject->activeSelf()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 1.00f));
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
    if (!gameObject->activeSelf()) {
        ImGui::PopStyleColor();
    }
}

void EditorGUI::DrawAxisIndicator()
{
    int w = RenderSystem::GetInstance().width();
    int h = RenderSystem::GetInstance().height();
    glViewport(0, 0, w*0.1f, h*0.1f);
    
    auto shader = axisIndicatorMaterial->shader();
    shader->Use();
    axisIndicatorMaterial->SetVector3("unity_LightPosition", Vector3(0, 0, 1));
    auto camera = Scene::mainCamera();
    //auto view = camera->worldToCameraMatrix();
    auto view = glm::lookAt(Vector3(0, 0, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));
    auto proj = camera->projectionMatrix();
    auto vp = proj * view;
    auto model = glm::mat4(glm::inverse(camera->transform()->rotation()));
    
    ShaderUniforms uniforms;
    axisIndicatorMaterial->SetMatrix("MATRIX_MVP", vp*model);
    axisIndicatorMaterial->SetMatrix("MATRIX_IT_MV", view*model);
    axisIndicatorMaterial->SetVector3("_Color", Vector3(1, 1, 1));
    shader->PreRender();
    axisIndicatorMaterial->Update();
    shader->CheckStatus();
    cubeMesh->Render();

    auto s = glm::scale(glm::mat4(), Vector3(0.5f, 0.75f, 0.5f));
    
    float f[] = {
        -1,  0,  0,    0, 0, -90,
         0, -1,  0,    0, 0,   0,
         0,  0, -1,   90, 0,   0,
         1,  0,  0,    0, 0,  90,
         0,  1,  0,  180, 0,   0,
         0,  0,  1,  -90, 0,   0,
    };
    Transform t;
    for (int i = 0; i < 6; ++i) {
        int j = i*6;
        t.setLocalPosition(f[j], f[j+1], f[j+2]);
        t.setLocalEulerAngles(f[j+3], f[j+4], f[j+5]);
        //t.Update();
        auto modelMat = model * t.localToWorldMatrix() * s;
        axisIndicatorMaterial->SetMatrix("MATRIX_MVP", vp*modelMat);
        axisIndicatorMaterial->SetMatrix("MATRIX_IT_MV", glm::inverse(glm::transpose(view*modelMat)));
        if (i >= 3)
            axisIndicatorMaterial->SetVector3("_Color", Vector3(f[j], f[j+1], f[j+2]));
        axisIndicatorMaterial->Update();
        coneMesh->Render();
    }
    
    shader->PostRender();
    
    auto v = camera->m_viewport;
    glViewport(w*v.x, h*v.y, w*v.z, h*v.w);
}
