#ifndef Transform_hpp
#define Transform_hpp

#include "RFGL.hpp"
#include "Log.hpp"
#include "Component.hpp"

// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
static Quaternion fromToRotation(const Vector3& fromDirection, const Vector3& toDirection)
{
    auto start = normalize(fromDirection);
    auto dest = normalize(toDirection);
    
    float cosTheta = dot(start, dest);
    Vector3 rotationAxis;
    
    if (cosTheta < -1 + 0.001f){
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = cross(Vector3(0.0f, 0.0f, 1.0f), start);
        if (glm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
            rotationAxis = cross(Vector3(1.0f, 0.0f, 0.0f), start);
        
        rotationAxis = normalize(rotationAxis);
        return glm::angleAxis(180.0f, rotationAxis);
    }
    
    rotationAxis = cross(start, dest);
    
    float s = sqrt( (1+cosTheta)*2 );
    float invs = 1 / s;
    
    return Quaternion(s * 0.5f,
                      rotationAxis.x * invs,
                      rotationAxis.y * invs,
                      rotationAxis.z * invs);
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
		update();
	}
    
    void update() const {
        //auto m = glm::translate(glm::scale(glm::mat4_cast(m_rotation), m_scale), m_position);
        m_matrix = glm::scale(glm::translate(glm::mat4_cast(m_rotation), m_position), m_scale);
        //m_matrix = glm::translate(glm::mat4(1.0f), m_position) * glm::scale(glm::mat4(1.0f), m_scale) * glm::mat4_cast(m_rotation);
        //m_isDirty = false;
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
//		if (m_isDirty)
//			update();
		return m_matrix;
	}

	// Rotates the transform so the forward vector points at /target/'s current position.
	void lookAt(const Vector3& target, const Vector3& worldUp = Vector3(0, 1,0))
    {
        //glm::lookAt(m_position, target, worldUp);
        Vector3 new_forward = glm::normalize(target - m_position);
    }
    
    // Applies a rotation of eulerAngles.z degrees around the z axis, eulerAngles.x degrees around the x axis, and eulerAngles.y degrees around the y axis (in that order).
    void rotate(Vector3 eulerAngles, Space relativeTo = Space::Space_Self);
    
    // Rotates the transform about axis passing through point in world coordinates by angle degrees.
    void rotateAround(const Vector3& point, const Vector3& axis, const float angle) {
        Vector3 r = glm::normalize(axis);
        float rad = glm::radians(angle);
        float c = cos(rad);
        float s = sin(rad);
        float one_c = 1-c;
        //glm::rotate(glm::quat(), rad, Vector3(0, 1, 0));
        Matrix4x4 mat(c+one_c*r.x*r.x, one_c*r.x*r.y-r.z*s, one_c*r.x*r.z+r.y*s, 0,
                      one_c*r.x*r.y+r.z*s, c+one_c*r.y*r.y, one_c*r.y*r.z-r.x*s, 0,
                      one_c*r.x*r.z-r.y*s, one_c*r.y*r.z+r.x*s, c+one_c*r.z*r.z, 0,
                      0, 0, 0, 1);
        mat = glm::transpose(mat);
        //mat = glm::translate(glm::mat4(1.0f), point) * mat * glm::translate(glm::mat4(1.0f), -point);

        //Matrix4x4 local2World = glm::inverse(m_matrix);
        //Quaternion rot1 = fromToRotation(old_forward, new_forward);
        //Vector3 old_forward = rot1 * Vector3(0, 0, 1);
        //Vector3 right = local2World * Vector3(1, 0, 0);
        //Vector3 new_forward = glm::normalize(point-m_position);
        //Vector3 new_forward = rot1 * Vector3(0, 0, 1);
        //m_rotation =  * m_rotation;
        //m_rotation = glm::rotate(glm::quat(), rad, Vector3(0, 1, 0)) * m_rotation;
        //info("%lf %lf %lf %lf", m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w);
        //mat = glm::translate(glm::mat4(1.0f), -point) * mat * glm::translate(glm::mat4(1.0f), point);
        m_position = mat*Vector4(m_position, 1.f);
        m_isDirty = true;
        //update();
    }
    
private:
	Vector3 m_position;
	Vector3 m_scale;
	Quaternion m_rotation;
	mutable Matrix4x4 m_matrix; // worldToLocal
	//mutable Matrix4x4 m_invMatrix;    // localToWorld
	mutable bool m_isDirty = false;
};

#endif // Transform_hpp