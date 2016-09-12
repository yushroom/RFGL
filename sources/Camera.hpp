//
//  Camera.hpp
//  PRT
//
//  Created by 俞云康 on 5/12/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include "RFGL.hpp"
#include "Behavior.hpp"
#include "GameObject.hpp"
#include "Transform.hpp"
#include "Input.hpp"

class CameraController : public Script
{
public:
    ClassName(CameraController)
    
    bool isRunning = false;
    float rotateSpeed = 40;
    float dragSpeed = 10;
    
    virtual void start() override
    {
        GUI::addBool("rotate", isRunning);
    }
    
    virtual void update() override
    {
        if (isRunning)
            m_gameObject->transform()->rotateAround(Vector3(0, 0, 0), Vector3(0, 1, 0), 1);
            
            if (Input::getMouseButton(1)) {
                float h = rotateSpeed * Input::getAxis(Input::Axis_MouseX);
                float v = rotateSpeed * Input::getAxis(Input::Axis_MouseY);
                m_transform->rotate(v, -h, 0);
                // fix "up" dirction
                m_transform->lookAt(m_transform->getForward() + m_transform->getPosition());
            }
        
        if (Input::getMouseButton(2)) {
            float x = dragSpeed * Input::getAxis(Input::Axis_MouseX);
            float y = dragSpeed * Input::getAxis(Input::Axis_MouseY);
            m_transform->translate(-x, -y, 0);
        }
    }
};

class Camera : public Behaviour
{
public:
	ClassName(Camera)

	Camera(float fov, float aspect, float zNear, float zFar) 
		: m_fov(fov), m_aspect(aspect), m_zNear(zNear), m_zFar(zFar) {
	}

	float getAspect() const {
		return m_aspect;
	}

	void setAspect(float aspect) {
		m_aspect = aspect;
	}

	Matrix4x4 getViewMatrix() const {
		return m_gameObject->transform()->getWorldToLocalMatrix();
	}

	Matrix4x4 getProjectMatrix() const {
		//return glm::perspective(glm::radians(m_fov), m_aspect, m_zNear, m_zFar);
		if (m_isDirty) {
			m_projectMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_zNear, m_zFar);
		}
		return m_projectMatrix;
	}

private:
	float m_fov;
	float m_aspect; // The aspect ratio (width divided by height).
	float m_zFar;
	float m_zNear;
	mutable bool m_isDirty = true;

	mutable Matrix4x4 m_projectMatrix;
};

#endif /* Camera_hpp */
