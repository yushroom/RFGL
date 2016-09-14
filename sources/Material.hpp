#ifndef Material_hpp
#define Material_hpp

#include "Object.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

class Material : public Object
{
public:
    Material() {
        
    }
    
    Material(Shader::PShader shader) {
        SetShader(shader);
    }
    
    void SetShader(std::shared_ptr<Shader> shader) {
        m_shader = shader;
        for (auto& u : m_shader->uniforms()) {
            if (u.type == GL_FLOAT) {
                m_uniforms.floats[u.name] = 0.5f;
            }
        }
    }
    
    typedef std::shared_ptr<Material> PMaterial;
    
    std::shared_ptr<Shader> shader() const {
        return m_shader;
    }
    
    // Set a named float value.
    void SetFloat(const std::string& name, float value) {
        //m_shader->BindUniformFloat(name.c_str(), value);
        m_uniforms.floats[name] = value;
    }
    
    // Set a named Vector3 value.
    void SetVector3(const std::string& name, const Vector3& value) {
        //m_shader->BindUniformVec3(name.c_str(), value);
        m_uniforms.vec3s[name] = value;
    }
    
    void BindTextures(const std::map<std::string, Texture::PTexture>& textures) {
        m_textures = textures;
    }

    auto uniforms() const {
        return m_uniforms;
    }
    
    ShaderUniforms& uniforms() {
        return m_uniforms;
    }
    
    void Update() {
        m_shader->BindUniforms(m_uniforms);
        m_shader->BindTextures(m_textures);
    }
    
    //========== Static Region ==========
    
    static void Init() {
        for (auto& s : std::vector<std::string>{"SkyBox", "NormalMap", "VisualizeNormal", "PBR"})
        {
            auto material = std::make_shared<Material>();
            material->SetShader(Shader::builtinShader(s));
            m_builtinMaterial[s] = material;
        }
    }
    
    static PMaterial CreateMaterial() {
        return std::make_shared<Material>();
    }
    
    static PMaterial builtinMaterial(const std::string& name) {
        auto it = m_builtinMaterial.find(name);
        if (it != m_builtinMaterial.end()) {
            return it->second;
        }
        Debug::LogWarning("No built-in material called %d", name.c_str());
        return nullptr;
    }

private:
    std::shared_ptr<Shader> m_shader = nullptr;
    std::map<std::string, Texture::PTexture> m_textures;
    
    ShaderUniforms m_uniforms;

    static std::map<std::string, PMaterial> m_builtinMaterial;
};


#endif /* Material_hpp */
