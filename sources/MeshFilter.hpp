//
//  MeshFilter.hpp
//  PRT
//
//  Created by 俞云康 on 9/10/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef MeshFilter_hpp
#define MeshFilter_hpp

#include "Component.hpp"
#include "GameObject.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "RenderSystem.hpp"

typedef Model Mesh;

class MeshFilter : public Component
{
public:
    ClassName(MeshFilter)
    
    MeshFilter() {
        
    }
    
    MeshFilter(std::shared_ptr<Mesh> mesh) : m_mesh(mesh) {
        
    }
    
    std::shared_ptr<Mesh> getMesh() const {
        return m_mesh;
    }
    
    void setMesh(std::shared_ptr<Mesh> mesh) {
        m_mesh = mesh;
    }
    
private:
    std::shared_ptr<Mesh> m_mesh = nullptr;
};

#endif /* MeshFilter_hpp */
