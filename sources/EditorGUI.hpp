#ifndef EditorGUI_hpp
#define EditorGUI_hpp

#include <string>
#include <functional>
//#include "imgui/imgui.h"

enum AssetType {
    AssetType_Model,
    AssetType_Material,
    AssetType_Texture,
    AssetType_AudioClip,
    AssetType_Font,
    AssetType_Script,
    AssetType_Shader,
};

class GameObject;
class Mesh;

class EditorGUI
{
public:
    EditorGUI() = delete;

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
    
    static void SelectMeshDialogBox(std::function<void(std::shared_ptr<Mesh>)> callback);
    
private:
    static int m_idCount;
    static void HierarchyItem(GameObject* gameObject);
    static void DrawAxisIndicator();
    
    static bool m_showAssectSelectionDialogBox;
};

#endif // EditorGUI_hpp