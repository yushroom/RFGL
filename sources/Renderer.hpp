//
//  Renderer.hpp
//  PRT
//
//  Created by 俞云康 on 9/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include "Component.hpp"
#include "Material.hpp"

class Renderer : public Component
{
public:
    ClassName(Renderer)
    
    Renderer() {
        
    }
    
    Renderer(Material::PMaterial material)
    {
        m_materials.push_back(material);
    }
    
    void addMaterial(std::shared_ptr<Material> material) {
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
    virtual void render() const = 0;
    std::vector<Material::PMaterial> m_materials;
};

#endif /* Renderer_hpp */
