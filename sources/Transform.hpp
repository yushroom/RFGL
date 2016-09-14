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
    
    Transform() : m_localPosition(0, 0, 0), m_localScale(1, 1, 1), m_localRotation(1, 0, 0, 0)
    {
    }


    void Start() {
    }
    
    virtual void OnEditorGUI() override {
        if (ImGui::InputFloat3("Position", glm::value_ptr(m_localPosition))) {
            m_isDirty = true;
        }
        if (ImGui::InputFloat3("Scale", glm::value_ptr(m_localScale))) {
            m_isDirty = true;
        }
        if (ImGui::InputFloat3("Rotation", glm::value_ptr(m_localEulerAngles))) {
            m_localRotation = glm::quat(glm::radians((m_localEulerAngles)));
            m_isDirty = true;
        }
    }

    // The position of the transform in world space.
    Vector3 position() const {
        if (m_isDirty)
            Update();
        return m_localToWorldMatrix * Vector4(0, 0, 0, 1);
    }

    // The global scale of the object(Read Only).
    Vector3 lossyScale() const {
        if (m_parent != nullptr)
            return m_localScale * m_parent->lossyScale();
        return m_localScale;
    }

    // The rotation of the transform in world space stored as a Quaternion.
    Quaternion rotation() const {
        if (m_isDirty)
            Update();
        if (m_parent != nullptr)
            return m_localRotation * m_parent->rotation();
        return m_localRotation;
    }

    // The rotation as Euler angles in degrees.
    Vector3 eulerAngles() const {
        if (m_isDirty)
            Update();
        return glm::eulerAngles(rotation());
    }

    // Position of the transform relative to the parent transform.
    Vector3 localPosition() const {
        return m_localPosition;
    }

    void setLocalPosition(const Vector3& position) {
        m_localPosition = position;
        m_isDirty = true;
    }

    void setLocalPosition(const float x, const float y, const float z) {
        m_localPosition.x = x;
        m_localPosition.y = y;
        m_localPosition.z = z;
        m_isDirty = true;
    }

    // The scale of the transform relative to the parent.
    Vector3 localScale() const {
        return m_localScale;
    }

    void setLocalScale(const Vector3& scale) {
        m_localScale = scale;
        m_isDirty = true;
    }

    void setLocalScale(const float x, const float y, const float z) {
        m_localScale.x = x;
        m_localScale.y = y;
        m_localScale.z = z;
        m_isDirty = true;
    }

    // The rotation of the transform relative to the parent transform's rotation.
    Quaternion localRotation() const {
        return glm::eulerAngles(m_localRotation);
        //return m_localRotation;
    }

    // The rotation as Euler angles in degrees relative to the parent transform's rotation.
    Vector3 localEulerAngles() const {
        if (m_isDirty)
            m_localEulerAngles = glm::degrees(glm::eulerAngles(m_localRotation));
        return m_localEulerAngles;
    }

    void setLocalEulerAngles(const Vector3& eulerAngles) {
        //m_localEulerAngles = eulerAngles;
        m_localRotation = glm::quat(glm::radians(eulerAngles));
        m_isDirty = true;
    }

    void setPosition(const Vector3& position) {
        //if (m_isDirty)
        //    Update();
        m_localToWorldMatrix = glm::scale(glm::translate(glm::mat4(1.0f), m_localPosition) * glm::mat4_cast(m_localRotation), m_localScale);
        m_worldToLocalMatrix = glm::inverse(m_localToWorldMatrix);
        if (m_parent == nullptr) {
            m_localPosition = m_worldToLocalMatrix * Vector4(position, 1);
        } else {
            m_localPosition = m_worldToLocalMatrix * m_parent->worldToLocalMatrix() * Vector4(position, 1) ;
        }
        m_isDirty = true;
    }

    void setPosition(const float x, const float y, const float z) {
        setPosition(Vector3(x, y, z));
    }

    //void setEulerAngles(const Vector3& eulerAngles) {
    //    m_localRotation = glm::quat(eulerAngles);
    //    m_isDirty = true;
    //}

    // Matrix that transforms a point from local space into world space (Read Only).
    Matrix4x4 localToWorldMatrix() const {
        Update();
        return m_localToWorldMatrix;
    }

    Matrix4x4 worldToLocalMatrix() const {
        Update();
        return m_worldToLocalMatrix;
    }

    Vector3 forward() const {
        Update();
        return rotation() * Vector3(0, 0, -1);
    }

    //void setForward(const Vector3& forward) {
    //    auto rot = fromToRotation(this->forward(), forward);
    //    //m_localRotation = rot * m_localRotation;
    //    m_isDirty = true;
    //}

    void Update() const {
        if (!m_isDirty)
            return;
        m_localEulerAngles = glm::degrees(glm::eulerAngles(m_localRotation));
        //m_localRotation = glm::quat(glm::radians(m_localEulerAngles));
        m_localToWorldMatrix = glm::scale(glm::translate(glm::mat4(1.0f), m_localPosition) * glm::mat4_cast(m_localRotation), m_localScale);
        if (m_parent != nullptr)
            m_localToWorldMatrix = m_parent->localToWorldMatrix() * m_localToWorldMatrix;
        m_worldToLocalMatrix = glm::inverse(m_localToWorldMatrix);

        //m_position = m_localToWorldMatrix * Vector4(0, 0, 0, 1);
        //m_forward = m_localRotation * Vector3(0, 0, -1);
        //m_right = m_localRotation * Vector3(1, 0, 0);
        m_isDirty = false;
        //m_matrix = glm::translate(glm::mat4(1.0f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale);
    }

    // Rotates the transform so the forward vector points at /target/'s current position.
    void LookAt(const Vector3& target, const Vector3& worldUp = Vector3(0, 1, 0))
    {
        m_worldToLocalMatrix = glm::lookAt(m_localPosition, target, worldUp);
        m_localToWorldMatrix = glm::inverse(m_worldToLocalMatrix);
        m_localRotation = glm::quat_cast(m_localToWorldMatrix);
        //m_localEulerAngles = glm::degrees(glm::eulerAngles(m_localRotation));
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
        Update();
        Quaternion lhs(glm::radians(eulerAngles));
        if (Space_Self == relativeTo) {
            auto r = this->rotation();
            m_localRotation = glm::inverse(r) * lhs * r * m_localRotation;
        }
        else {
            m_localRotation = lhs * m_localRotation;
        }
        //m_localEulerAngles = glm::degrees(glm::eulerAngles(m_localRotation));
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
        //m_localEulerAngles = glm::degrees(glm::eulerAngles(m_localRotation));
        LookAt(point);
    }

    // The parent of the transform.
    Transform* parent() const {
        return m_parent;
    }

    void setParent(Transform* parent) {
        m_parent = parent;
        m_isDirty = true;
    }
    
private:
    //mutable Vector3 m_position;
    //mutable Vector3 m_scale;
    //mutable Quaternion m_rotation;
    //mutable Vector3 m_eulerAngles;

    Vector3 m_localPosition;
    Vector3 m_localScale;
    Quaternion m_localRotation;
    mutable Vector3 m_localEulerAngles;

    Transform* m_parent = nullptr;

    mutable bool m_isDirty = true;
    //mutable Vector3 m_right;	// (1, 0, 0) in local space
    //mutable Vector3 m_forward;	// (0, 0, -1) in local space
    
    mutable Matrix4x4 m_localToWorldMatrix; // localToWorld
    mutable Matrix4x4 m_worldToLocalMatrix; // worldToLocal
};

#endif // Transform_hpp