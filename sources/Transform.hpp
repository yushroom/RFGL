#ifndef Transform_hpp
#define Transform_hpp

#include "RFGL.hpp"
#include "Component.hpp"

class Transform : public Component
{
public:
	ClassName(Transform)

	Transform() : m_position(0, 0, 0), m_scale(1, 1, 1), m_rotation(1, 0, 0, 0) {
		//m_matrix = glm::translate(glm::scale(glm::mat4_cast(m_rotation), m_scale), m_position);
		update();
	}

	Vector3 getPosition() const {
		return m_position;
	}

	Vector3 getScale() const {
		return m_scale;
	}

	Quaternion getRotation() const {
		return m_rotation;
	}

	// The rotation as Euler angles in degrees.
	Vector3 getEulerAngles() const {
		return glm::eulerAngles(m_rotation);
	}

	void setPosition(const Vector3& position) {
		m_position = position;
		m_isDirty = true;
	}

	void setPosition(const float x, const float y, const float z) {
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
		m_isDirty = true;
	}

	void setScale(const Vector3& scale) {
		m_scale = scale;
		m_isDirty = true;
	}

	void setScale(const float x, const float y, const float z) {
		m_scale.x = x;
		m_scale.y = y;
		m_scale.z = z;
		m_isDirty = true;
	}


	void setEulerAngles(const Vector3& eulerAngles) {
		m_rotation = glm::quat(eulerAngles);
		m_isDirty = true;
	}

	Matrix4x4 getMatrix() const {
		if (m_isDirty)
			update();
		return m_matrix;
	}

	// Rotates the transform so the forward vector points at /target/'s current position.
	void lookAt(const Transform& target, const Vector3& worldUp);

private:
	Vector3 m_position;
	Vector3 m_scale;
	Quaternion m_rotation;
	mutable Matrix4x4 m_matrix;
	//mutable Matrix4x4 m_invMatrix;
	mutable bool m_isDirty = false;

	void update() const {
		//auto m = glm::translate(glm::scale(glm::mat4_cast(m_rotation), m_scale), m_position);
		m_matrix = glm::scale(glm::translate(glm::mat4_cast(m_rotation), m_position), m_scale);
		//m_matrix = glm::translate(glm::mat4(1.0f), m_position) * glm::scale(glm::mat4(1.0f), m_scale) * glm::mat4_cast(m_rotation);
		m_isDirty = false;
	}
};

#endif // Transform_hpp