//
//  MeshRenderer.hpp
//  PRT
//
//  Created by 俞云康 on 9/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef MeshRenderer_hpp
#define MeshRenderer_hpp

#include "Renderer.hpp"
#include "MeshFilter.hpp"
#include "Scene.hpp"

class MeshRenderer : public Renderer
{
public:
    ClassName(MeshRenderer)
    
    MeshRenderer() {
        
    }
    
    MeshRenderer(Material::PMaterial material) : Renderer(material)
    {
        
    }
    
    //protected:
    virtual void render() const override {
        auto meshFilter = m_gameObject->getComponent<MeshFilter>();
        if (meshFilter == nullptr) {
            Debug::LogWarning("This GameObject has no MeshFilter");
            return;
        }
        
        auto model = m_transform->getLocalToWorldMatrix();
        auto camera = Scene::getMainCamera();
        auto view = camera->getViewMatrix();
        auto proj = camera->getProjectMatrix();
        auto mv = view * model;
        auto mvp = proj * mv;

        BuiltinShaderUniforms uniforms;
        //std::map<std::string, Matrix4x4> matrices;
        uniforms.mat4s["_Object2World"] = model;
        uniforms.mat4s["MATRIX_V"] = view;
        uniforms.mat4s["MATRIX_P"] = proj;
        uniforms.mat4s["MATRIX_VP"] = proj * view;
        uniforms.mat4s["MATRIX_MVP"] = mvp;
        uniforms.mat4s["MATRIX_MV"] = mv;
        uniforms.mat4s["MATRIX_IT_MV"] = glm::inverse(glm::transpose(mv));
        //auto camera = Scene::getMainCamera();
        uniforms.vec3s["_WorldSpaceCameraPos"] = camera->transform()->getPosition();
        
        for (auto& m : m_materials) {
            auto shader = m->shader();
            shader->use();
            shader->preRender();
            shader->bindBuiltinUniforms(uniforms);
            //shader->bindAllTexture();
            //for (auto& m : m_materials)
            m->update();
            meshFilter->getMesh()->render();
            shader->postRender();
        }
    }
};

#endif /* MeshRenderer_hpp */
