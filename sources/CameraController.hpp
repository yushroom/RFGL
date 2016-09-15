#ifndef CameraController_hpp
#define CameraController_hpp

#include "Input.hpp"

class CameraController : public Script
{
public:
    InjectClassName(CameraController)
    
    bool m_isRotating = false;
    float m_rotateSpeed = 60;
    float m_dragSpeed = 10;
    
    virtual void Start() override
    {
        //GUI::AddBool("rotate", m_isRotating);
    }
    
    virtual void OnEditorGUI() override {
        ImGui::Checkbox("rotate camera", &m_isRotating);
    }
    
    virtual void Update() override
    {
        if (m_isRotating)
            m_gameObject->transform()->RotateAround(Vector3(0, 0, 0), Vector3(0, 1, 0), 1);
            
            if (Input::GetMouseButton(1)) {
                float h = m_rotateSpeed * Input::GetAxis(Input::Axis_MouseX);
                float v = m_rotateSpeed * Input::GetAxis(Input::Axis_MouseY);
                transform()->Rotate(v, -h, 0);
                // fix "up" dirction
                transform()->LookAt(transform()->forward() + transform()->position());
            }
        
        if (Input::GetMouseButton(2)) {
            float x = m_dragSpeed * Input::GetAxis(Input::Axis_MouseX);
            float y = m_dragSpeed * Input::GetAxis(Input::Axis_MouseY);
            transform()->Translate(-x, -y, 0);
        }
    }
};

#endif //CameraController_hpp