#ifndef Transform_hpp
#define Transform_hpp

#include "RFGL.hpp"
#include "Component.hpp"
#include "GUI.hpp"

// Creates a rotation which rotates angle degrees around axis.
static Quaternion angleAxis(float angle, Vector3 axis)
{
	axis = glm::normalize(axis);
	angle = glm::radians(angle);
	return glm::angleAxis(angle, axis);
}

// Creates a rotation which rotates from fromDirection to toDirection.
static Quaternion fromToRotation(const Vector3& fromDirection, const Vector3& toDirection)
{
    auto start = normalize(fromDirection);
    auto dest = normalize(toDirection);
	return glm::rotation(start, dest);
}

class Transform : public Component
{
public:
    InjectClassName(Transform)
    
    enum Space {
        Space_World = 0,
        Space_Self
    };
    
	Transform();

	void Start() {
		//GUI::addFloat("tx", m_position.x);
		//GUI::addFloat("ty", m_position.y);
		//GUI::addFloat("tz", m_position.z);
		//GUI::addFloat("rx", m_eulerAngles.x);
		//GUI::addFloat("ry", m_eulerAngles.x);
		//GUI::addFloat("rz", m_eulerAngles.x);
		//GUI::addVector3("forward", m_forward);
	}

	Vector3 position() const {
		return m_localPosition;
	}

	Vector3 scale() const {
		return m_localScale;
	}

	Quaternion rotation() const {
		return m_localRotation;
	}

	// The rotation as Euler angles in degrees.
	Vector3 eulerAngles() const {
		if (m_isDirty)
			Update();
		return m_localEulerAngles;
	}

	void setPosition(const Vector3& position) {
		m_localPosition = position;
		m_isDirty = true;
	}

	void setPosition(const float x, const float y, const float z) {
		m_localPosition.x = x;
		m_localPosition.y = y;
		m_localPosition.z = z;
		m_isDirty = true;
	}

	void setScale(const Vector3& scale) {
		m_localScale = scale;
		m_isDirty = true;
	}

	void setScale(const float x, const float y, const float z) {
		m_localScale.x = x;
		m_localScale.y = y;
		m_localScale.z = z;
		m_isDirty = true;
	}

	void setEulerAngles(const Vector3& eulerAngles) {
		m_localRotation = glm::quat(eulerAngles);
		m_isDirty = true;
	}

	Matrix4x4 localToWorldMatrix() const {
		if (m_isDirty)
			Update();
		return m_localToWorldMatrix;
	}

	Matrix4x4 worldToLocalMatrix() const {
		if (m_isDirty)
			Update();
		return m_worldToLocalMatrix;
	}

	Vector3 forward() const {
		if (m_isDirty)
			Update();
		return m_forward;
	}

	void setForward(const Vector3& forward) {
		auto rot = fromToRotation(m_forward, forward);
		m_localRotation = rot * m_localRotation;
		m_isDirty = true;
	}

	void Update() const {
		m_localEulerAngles = glm::eulerAngles(m_localRotation);
        m_localToWorldMatrix = glm::scale(glm::translate(glm::mat4(1.0f), m_localPosition) * glm::mat4_cast(m_localRotation), m_localScale);
        if (m_parent != nullptr)
            m_localToWorldMatrix = m_parent->localToWorldMatrix() * m_localToWorldMatrix;
		m_worldToLocalMatrix = glm::inverse(m_localToWorldMatrix);

        m_position = m_localToWorldMatrix * Vector4(0, 0, 0, 1);
        
        
		m_forward = m_localRotation * Vector3(0, 0, -1);
		m_right = m_localRotation * Vector3(1, 0, 0);
		m_isDirty = false;
		//m_matrix = glm::translate(glm::mat4(1.0f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale);
	}

	// Rotates the transform so the forward vector points at /target/'s current position.
	void LookAt(const Vector3& target, const Vector3& worldUp = Vector3(0, 1, 0))
    {
		m_worldToLocalMatrix = glm::lookAt(m_localPosition, target, worldUp);
		m_localToWorldMatrix = glm::inverse(m_worldToLocalMatrix);
		m_localRotation = glm::quat_cast(m_localToWorldMatrix);
		m_isDirty = true;
    }

	// Transforms direction from local space to world space.
	Vector3 TransformDirection(const Vector3& direction)
	{
		return m_localToWorldMatrix * Vector4(direction, 0);
	}

	void Translate(const Vector3& translation, Space relativeTo = Space_Self) {
		if (relativeTo == Space_World)
			m_localPosition += translation;
		else
			m_localPosition += TransformDirection(translation);
		m_isDirty = true;
	}

	void Translate(float x, float y, float z, Space relativeTo = Space_Self) {
		Translate(Vector3(x, y, z), relativeTo);
	}
     
    // Applies a rotation of eulerAngles.z degrees around the z axis, eulerAngles.x degrees around the x axis, and eulerAngles.y degrees around the y axis (in that order).
	void Rotate(Vector3 eulerAngles, Space relativeTo = Space::Space_Self)
	{
		Quaternion lhs(glm::radians(eulerAngles));
		//if (Space_Self == relativeTo) {
		//	m_rotation *= 
		//}
		//else {
		//
		//}
		m_localRotation = lhs * m_localRotation;
		m_isDirty = true;
		//m_rotation = glm::inverse(m_rotation) * rhs * m_rotation * m_rotation;
	}
    
	// Applies a rotation of zAngle degrees around the z axis, xAngle degrees around the x axis, and yAngle degrees around the y axis (in that order)
	void Rotate(float xAngle, float yAngle, float zAngle, Space relativeTo = Space::Space_Self) {
		this->Rotate(Vector3(xAngle, yAngle, zAngle), relativeTo);
	}

    // Rotates the transform about axis passing through point in world coordinates by angle degrees.
    void RotateAround(const Vector3& point, const Vector3& axis, float angle) {
		auto rotation = angleAxis(angle, axis);
		m_localPosition = point + rotation * (m_localPosition - point);
		LookAt(point);
    }

	Transform* parent() const {
		return m_parent;
	}

	void setParent(Transform* parent) {
		m_parent = parent;
	}
    
private:
	mutable Vector3 m_position;
	mutable Vector3 m_scale;
	mutable Quaternion m_rotation;
    mutable Vector3 m_eulerAngles;

	Vector3 m_localPosition;
	Vector3 m_localScale;
	Quaternion m_localRotation;
    mutable Vector3 m_localEulerAngles;

	Transform* m_parent = nullptr;

	mutable bool m_isDirty = true;
	mutable Vector3 m_right;	// (1, 0, 0) in local space
	mutable Vector3 m_forward;	// (0, 0, -1) in local space
	
	mutable Matrix4x4 m_localToWorldMatrix; // localToWorld
	mutable Matrix4x4 m_worldToLocalMatrix; // worldToLocal
};

#endif // Transform_hpp