#ifndef Renderer_hpp
#define Renderer_hpp

#include "Component.hpp"
#include "Material.hpp"

class Renderer : public Component
{
public:
    InjectClassName(Renderer)
    
    Renderer() {
        
    }
    
    Renderer(Material::PMaterial material)
    {
        m_materials.push_back(material);
    }
    
    void AddMaterial(std::shared_ptr<Material> material) {
        m_materials.push_back(material);
    }
    
    Material::PMaterial material() const {
        if (m_materials.size() > 0)
            return m_materials[0];
        return nullptr;
    }
    
    std::vector<Material::PMaterial>& materials() {
        return m_materials;
    }
    
    
protected:
    virtual void Render() const = 0;
    std::vector<Material::PMaterial> m_materials;
};

#endif /* Renderer_hpp */
