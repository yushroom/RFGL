#ifndef MeshFilter_hpp
#define MeshFilter_hpp

#include "Component.hpp"
#include "GameObject.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "RenderSystem.hpp"

class MeshFilter : public Component
{
public:
    InjectClassName(MeshFilter)
    
    MeshFilter() = default;
    
    MeshFilter(std::shared_ptr<Mesh> mesh) : m_mesh(mesh) {
        
    }
    
    std::shared_ptr<Mesh> mesh() const {
        return m_mesh;
    }
    
    void SetMesh(std::shared_ptr<Mesh> mesh) {
        m_mesh = mesh;
    }
    
    virtual void OnEditorGUI() override {
        ImGui::Text("%s", m_mesh->name().c_str());
    }
    
private:
    std::shared_ptr<Mesh> m_mesh = nullptr;
};

#endif /* MeshFilter_hpp */
