#ifndef Transform_hpp
#define Transform_hpp

#include "RFGL.hpp"
#include "Log.hpp"
#include "Component.hpp"
#include "GUI.hpp"


static void log(const Vector3& v) {
	info("Vector3(%lf, %lf, %lf)", v.x, v.y, v.z);
}

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
	ClassName(Transform)
    
    enum Space {
        Space_World = 0,
        Space_Self
    };
    
	Transform() : m_position(0, 0, 0), m_scale(1, 1, 1), m_rotation(1, 0, 0, 0) {
	}

	void start() {
		//GUI::addFloat("tx", m_position.x);
		//GUI::addFloat("ty", m_position.y);
		//GUI::addFloat("tz", m_position.z);
		//GUI::addFloat("rx", m_eulerAngles.x);
		//GUI::addFloat("ry", m_eulerAngles.x);
		//GUI::addFloat("rz", m_eulerAngles.x);
		//GUI::addVector3("forward", m_forward);
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
		if (m_isDirty)
			update();
		return m_eulerAngles;
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

	Matrix4x4 getWorld2LocalMatrix() const {
		if (m_isDirty)
			update();
		return m_world2LocalMatrix;
	}

	Vector3 getForward() const {
		if (m_isDirty)
			update();
		return m_forward;
	}

	void setForward(const Vector3& forward) {
		auto rot = fromToRotation(m_forward, forward);
		m_rotation = rot * m_rotation;
		m_isDirty = true;
	}

	void update() const {
		m_eulerAngles = glm::eulerAngles(m_rotation);
		m_local2WorldMatrix = glm::scale(glm::translate(glm::mat4(1.0f), m_position) * glm::mat4_cast(m_rotation), m_scale);
		m_world2LocalMatrix = glm::inverse(m_local2WorldMatrix);
		m_forward = m_rotation * Vector3(0, 0, -1);
		m_right = m_rotation * Vector3(1, 0, 0);
		m_isDirty = false;
		//m_matrix = glm::translate(glm::mat4(1.0f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale);
	}

	// Rotates the transform so the forward vector points at /target/'s current position.
	void lookAt(const Vector3& target, const Vector3& worldUp = Vector3(0, 1, 0))
    {
		m_world2LocalMatrix = glm::lookAt(m_position, target, worldUp);
		m_local2WorldMatrix = glm::inverse(m_world2LocalMatrix);
		m_rotation = glm::quat_cast(m_local2WorldMatrix);
		m_isDirty = true;
		//update();
    }

	/// <summary>
	///   <para>Transforms direction from local space to world space.</para>
	/// </summary>
	/// <param name="direction"></param>
	Vector3 TransformDirection(const Vector3& direction)
	{
		log(direction);
		Vector3 result = m_local2WorldMatrix * Vector4(direction, 0);
		//log(result);
		return result;
	}

	void translate(const Vector3& translation, Space relativeTo = Space_Self) {
		if (relativeTo == Space_World)
			m_position += translation;
		else
			m_position += TransformDirection(translation);
		m_isDirty = true;
	}

	void translate(float x, float y, float z, Space relativeTo = Space_Self) {
		translate(Vector3(x, y, z), relativeTo);
	}
     
    // Applies a rotation of eulerAngles.z degrees around the z axis, eulerAngles.x degrees around the x axis, and eulerAngles.y degrees around the y axis (in that order).
	void rotate(Vector3 eulerAngles, Space relativeTo = Space::Space_Self)
	{
		Quaternion lhs(glm::radians(eulerAngles));
		//if (Space_Self == relativeTo) {
		//	m_rotation *= 
		//}
		//else {
		//
		//}
		m_rotation = lhs * m_rotation;
		m_isDirty = true;
		//m_rotation = glm::inverse(m_rotation) * rhs * m_rotation * m_rotation;
	}
    
	// Applies a rotation of zAngle degrees around the z axis, xAngle degrees around the x axis, and yAngle degrees around the y axis (in that order)
	void rotate(float xAngle, float yAngle, float zAngle, Space relativeTo = Space::Space_Self) {
		this->rotate(Vector3(xAngle, yAngle, zAngle), relativeTo);
	}

    // Rotates the transform about axis passing through point in world coordinates by angle degrees.
    void rotateAround(const Vector3& point, const Vector3& axis, float angle) {
		auto rotation = angleAxis(angle, axis);
		m_position = point + rotation * (m_position - point);
		lookAt(point);
    }
    
private:
	Vector3 m_position;
	Vector3 m_scale;
	Quaternion m_rotation;

	mutable bool m_isDirty = true;

	mutable Vector3 m_right;	// (1, 0, 0) in local space
	mutable Vector3 m_forward;	// (0, 0, -1) in local space
	mutable Vector3 m_eulerAngles;
	
	mutable Matrix4x4 m_local2WorldMatrix; // localToWorld
	mutable Matrix4x4 m_world2LocalMatrix; // worldToLocal
};

#endif // Transform_hpp