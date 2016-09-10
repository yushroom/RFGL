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

class Camera : public Behaviour
{
public:
	ClassName(Camera)

	Camera(float fov, float aspect, float zNear, float zFar) 
		: m_fov(fov), m_aspect(aspect), m_zNear(zNear), m_zFar(zFar) {
		//m_projectMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_zNear, m_zFar);
	}

	float getAspect() const {
		return m_aspect;
	}

	void setAspect(float aspect) {
		m_aspect = aspect;
	}

	Matrix4x4 getViewMatrix() const {
		return gameObject->transform.getWorldToLocalMatrix();
		//return gameObject->transform.getMatrix();
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

//// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
//enum Camera_Movement {
//    FORWARD,
//    BACKWARD,
//    LEFT,
//    RIGHT
//};
//
//// Default camera values
//const GLfloat YAW        = -90.0f;
//const GLfloat PITCH      =  0.0f;
//const GLfloat SPEED      =  3.0f;
//const GLfloat SENSITIVTY =  0.25f;
//const GLfloat ZOOM       =  60.0f;


//// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
//class Camera
//{
//public:
//    // Camera Attributes
//    glm::vec3 Position;
//    glm::vec3 Front;
//    glm::vec3 Up;
//    glm::vec3 Right;
//    glm::vec3 WorldUp;
//    // Eular Angles
//    GLfloat Yaw;
//    GLfloat Pitch;
//    // Camera options
//    GLfloat MovementSpeed;
//    GLfloat MouseSensitivity;
//    GLfloat Zoom;
//    
//    // Constructor with vectors
//    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
//    {
//        this->Position = position;
//        this->WorldUp = up;
//        this->Yaw = yaw;
//        this->Pitch = pitch;
//        this->updateCameraVectors();
//    }
//    // Constructor with scalar values
//    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
//    {
//        this->Position = glm::vec3(posX, posY, posZ);
//        this->WorldUp = glm::vec3(upX, upY, upZ);
//        this->Yaw = yaw;
//        this->Pitch = pitch;
//        this->updateCameraVectors();
//    }
//    
//    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
//    glm::mat4 GetViewMatrix()
//    {
//        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
//    }
//    
//    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
//    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
//    {
//        GLfloat velocity = this->MovementSpeed * deltaTime;
//        if (direction == FORWARD)
//            this->Position += this->Front * velocity;
//        if (direction == BACKWARD)
//            this->Position -= this->Front * velocity;
//        if (direction == LEFT)
//            this->Position -= this->Right * velocity;
//        if (direction == RIGHT)
//            this->Position += this->Right * velocity;
//    }
//    
//    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
//    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
//    {
//        xoffset *= this->MouseSensitivity;
//        yoffset *= this->MouseSensitivity;
//        
//        this->Yaw   += xoffset;
//        this->Pitch += yoffset;
//        
//        // Make sure that when pitch is out of bounds, screen doesn't get flipped
//        if (constrainPitch)
//        {
//            if (this->Pitch > 89.0f)
//                this->Pitch = 89.0f;
//            if (this->Pitch < -89.0f)
//                this->Pitch = -89.0f;
//        }
//        
//        // Update Front, Right and Up Vectors using the updated Eular angles
//        this->updateCameraVectors();
//    }
//    
//    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
//    void ProcessMouseScroll(GLfloat yoffset)
//    {
//        const float MAX_FOV = 90.f;
//        if (this->Zoom >= 1.0f && this->Zoom <= MAX_FOV)
//            this->Zoom -= yoffset;
//        if (this->Zoom <= 1.0f)
//            this->Zoom = 1.0f;
//        else if (this->Zoom >= MAX_FOV)
//            this->Zoom = MAX_FOV;
//        //printf("%lf\n", this->Zoom);
//    }
//    
//private:
//    // Calculates the front vector from the Camera's (updated) Eular Angles
//    void updateCameraVectors()
//    {
//        // Calculate the new Front vector
//        glm::vec3 front;
//        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
//        front.y = sin(glm::radians(this->Pitch));
//        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
//        this->Front = glm::normalize(front);
//        // Also re-calculate the Right and Up vector
//        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
//    }
//};

//class LookAtCamera
//{
//public:
//	LookAtCamera(const glm::vec3& at, const glm::vec3& eye)
//		: m_at(at)
//	{
//		m_eyeDirection = m_at - eye;
//		m_distance = m_eyeDirection.length();
//		m_eyeDirection /= m_distance;
//	}
//
//	glm::mat4 GetViewMatrix()
//	{
//		return glm::lookAt(m_at - m_eyeDirection*m_distance, m_at, glm::vec3(0, 1, 0));
//	}
//
//	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
//	{
//
//	}
//
//	void ProcessMouseScroll(float deltaDistance) {
//		m_distance += deltaDistance;
//		m_distance = glm::clamp(m_distance, m_minDistance, m_maxDistance);
//	}
//
//private:
//	float m_maxDistance = 1;
//	float m_minDistance = 10;
//	glm::vec3 m_at;
//	glm::vec3 m_eyeDirection;
//	float m_distance;
//};

#endif /* Camera_hpp */
