#ifndef MeshRenderer_hpp
#define MeshRenderer_hpp

#include "Renderer.hpp"
#include "MeshFilter.hpp"
#include "Scene.hpp"
#include "GLError.hpp"

class MeshRenderer : public Renderer
{
public:
    InjectClassName(MeshRenderer)
    
    MeshRenderer() = default;
    
    MeshRenderer(Material::PMaterial material) : Renderer(material)
    {
        
    }
    
    //protected:
    virtual void Render() const override {
        //Debug::Log("Rendere %s", m_gameObject->name().c_str());
        auto meshFilter = m_gameObject->GetComponent<MeshFilter>();
        if (meshFilter == nullptr) {
            Debug::LogWarning("This GameObject has no MeshFilter");
            return;
        }
        
        auto model = transform()->localToWorldMatrix();
        auto camera = Scene::mainCamera();
        auto view = camera->worldToCameraMatrix();
        auto proj = camera->projectionMatrix();
        auto mv = view * model;
        auto mvp = proj * mv;

        ShaderUniforms uniforms;
        //std::map<std::string, Matrix4x4> matrices;
        uniforms.mat4s["_Object2World"] = model;
        uniforms.mat4s["MATRIX_V"] = view;
        uniforms.mat4s["MATRIX_P"] = proj;
        uniforms.mat4s["MATRIX_VP"] = proj * view;
        uniforms.mat4s["MATRIX_MVP"] = mvp;
        uniforms.mat4s["MATRIX_MV"] = mv;
        uniforms.mat4s["MATRIX_IT_M"] = glm::inverse(glm::transpose(model));
        uniforms.mat4s["MATRIX_IT_MV"] = glm::inverse(glm::transpose(mv));
        //auto camera = Scene::getMainCamera();
        uniforms.vec3s["_WorldSpaceCameraPos"] = camera->transform()->position();
        
        for (auto& m : m_materials) {
            auto shader = m->shader();
            shader->Use();
            shader->PreRender();
            shader->BindUniforms(uniforms);
            m->Update();
            shader->CheckStatus();
            meshFilter->mesh()->Render();
            shader->PostRender();
        }
    }
};

#endif /* MeshRenderer_hpp */
