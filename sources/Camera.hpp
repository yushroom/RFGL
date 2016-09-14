#ifndef Camera_hpp
#define Camera_hpp

#include "RFGL.hpp"
#include "Behaviour.hpp"
#include "GameObject.hpp"
#include "Transform.hpp"
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

class Scene;

class Camera : public Behaviour
{
public:
    InjectClassName(Camera)

    Camera(float fov, float aspect, float zNear, float zFar) 
        : m_fov(fov), m_aspect(aspect), m_zNear(zNear), m_zFar(zFar) {
    }

    // The aspect ratio (width divided by height).
    float aspect() const {
        return m_aspect;
    }

    void setAspect(float aspect) {
        m_aspect = aspect;
    }

    // Matrix that transforms from world to camera space (i.e. view matrix).
    Matrix4x4 worldToCameraMatrix() const {
        return m_gameObject->transform()->worldToLocalMatrix();
    }

    // projection matrix.
    Matrix4x4 projectionMatrix() const {
        //return glm::perspective(glm::radians(m_fov), m_aspect, m_zNear, m_zFar);
        if (m_isDirty) {
            m_projectMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_zNear, m_zFar);
        }
        return m_projectMatrix;
    }

    // TODO
    // The first enabled camera tagged "MainCamera" (Read Only).
    static std::shared_ptr<Camera> main();

private:
    float m_fov;
    float m_aspect; // The aspect ratio (width divided by height).
    float m_zFar;
    float m_zNear;
    mutable bool m_isDirty = true;

    mutable Matrix4x4 m_projectMatrix;
};

#endif /* Camera_hpp */
