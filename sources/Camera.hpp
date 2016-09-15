#ifndef Camera_hpp
#define Camera_hpp

#include "Behaviour.hpp"
#include "GameObject.hpp"

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

    virtual void OnEditorGUI() override {
        ImGui::SliderFloat("Field of View", &m_fov, 1, 179);
        ImGui::InputFloat("Clipping Planes(Near)", &m_zNear);
        ImGui::InputFloat("Clipping Planes(Far)", &m_zFar);
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
