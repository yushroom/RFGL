//
//  Texture.hpp
//  PRT
//
//  Created by 俞云康 on 5/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef Shader_hpp
#define Shader_hpp

#include "RFGL.hpp"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Debug.hpp"

#define STRING_MACRO(x) x

#define PositionIndex 0
#define NormalIndex 1
#define UVIndex 2
#define TangentIndex 3

// Unity Built-in shader variables
// http://docs.unity3d.com/Manual/SL-UnityShaderVariables.html

const static std::string ShaderMacro = R"(#version 410 core
#define PositionIndex 0
#define NormalIndex 1
#define UVIndex 2
#define TangentIndex 3

// Transformations
uniform mat4 MATRIX_MVP;
//uniform mat4 MATRIX_MV;
uniform mat4 MATRIX_V;
uniform mat4 MATRIX_P;
uniform mat4 MATRIX_VP;
//uniform mat4 MATRIX_T_MV;
uniform mat4 MATRIX_IT_MV;
uniform mat4 _Object2World;
//uniform mat4 _World2Object;

// Camera and screen
uniform vec3 _WorldSpaceCameraPos;
//uniform vec4 _ProjectionParams;
//uniform vec4 _ScreenParams;
//uniform vec4 _ZBufferParams;
//uniform vec4 OrthoParams;
//uniform mat4 CameraProjection;
//uniform mat4 CameraInvProjection;
//uniform vec4 _CameraWorldClipPlanes[6];

// Time
//uniform vec4 _Time;
//uniform vec4 _SinTime;
//uniform vec4 _CosTime;
//uniform vec4 _DeltaTime;
)";

struct BuiltinShaderUniforms
{
    std::map<std::string, Matrix4x4> mat4s;
    std::map<std::string, Vector3> vec3s;
};

class RenderSystem;

class Shader
{
public:
    Shader() = default;
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
	Shader(Shader&&);

    typedef std::shared_ptr<Shader> PShader;

    static PShader createFromString(const std::string& vs_str, const std::string& fs_str) {
        auto s = std::make_shared<Shader>();
        s->fromString(vs_str, fs_str);
        return s;
    }
    
    static PShader createFromString(const std::string& vs_str, const std::string& fs_str, const std::string& gs_str) {
        auto s = std::make_shared<Shader>();
        s->fromString(vs_str, fs_str, gs_str);
        return s;
    }

    void fromString(const std::string& vs_str, const std::string& fs_str);
    void fromString(const std::string& vs_str, const std::string& fs_str, const std::string& gs_str);
    void fromString(const std::string& vs_str,
                    const std::string& tcs_str,
                    const std::string& tes_str,
                    const std::string& gs_str,
                    const std::string& fs_str);
    void fromFile(const std::string& vs_path, const std::string ps_path);
	//Shader(const std::string& vs_path, const std::string ps_path);
    ~Shader();
    
    void use() const;
    
    GLuint getGLProgram() const {
        return m_program;
    }
    
    GLuint getAttribLocation(const char* name) const;
    
    void bindUniformFloat(const char* name, const float value) const;
    void bindUniformVec3(const char* name, const glm::vec3& value) const;
    void bindUniformMat3(const char* name, const glm::mat3& value) const;
    void bindUniformMat4(const char* name, const glm::mat4& value) const;
    
    void bindUniformTexture(const char* name, const GLuint texture, const GLuint id, GLenum textureType = GL_TEXTURE_2D) const;

    void bindBuiltinUniforms(const BuiltinShaderUniforms& uniforms) const {
        for (auto& u : m_uniforms) {
            if (u.type == GL_FLOAT_MAT4) {
                auto it = uniforms.mat4s.find(u.name);
                if (it != uniforms.mat4s.end()) {
                    bindUniformMat4(u.name.c_str(), it->second);
                } else {
                    Debug::LogWarning("%s of type %u not found", u.name.c_str(), u.type);
                }
            } else if (u.type == GL_FLOAT_VEC3) {
                auto it = uniforms.vec3s.find(u.name);
                if (it != uniforms.vec3s.end()) {
                    bindUniformVec3(u.name.c_str(), it->second);
                } else {
                    Debug::LogWarning("%s of type %u not found", u.name.c_str(), u.type);
                }
            }
        }
    }
    
    void bindAllTexture(const std::map<std::string, GLuint>& textures) const {
        int texture_id = 0;
        for (auto& u : m_uniforms) {
            if (u.type != GL_SAMPLER_2D && u.type != GL_SAMPLER_CUBE) continue;
            auto it = textures.find(u.name);
            if (it != textures.end()) {
                GLenum type = u.type==GL_SAMPLER_2D ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP;
                bindUniformTexture(u.name.c_str(), it->second, texture_id, type);
                texture_id ++;
            } else {
                Debug::LogWarning("%s of type %u not found", u.name.c_str(), u.type);
            }
        }
    }
    
    void preRender() const {
        glCullFace(m_cullface);
        glDepthMask(m_ZWrite);
    }
    
    void postRender() const {
        glDepthMask(GL_TRUE);
        glCullFace(GL_BACK);
    }
    
    static PShader getBuiltinShader(const std::string& name) {
        auto it = m_builtinShaders.find(name);
        if (it != m_builtinShaders.end()) {
            return it->second;
        }
        Debug::LogWarning("No builtin shader called %d", name.c_str());
        return nullptr;
    }
    
private:
	GLuint m_program = 0;
    
    GLint _getUniformLocation(const char* name) const;
    
    struct UniformInfo {
        //GLint size; // size of the variable
        GLenum type; // type of the variable (float, vec3 or mat4, etc)
        //GLchar name[32];
        std::string name;  // variable name in GLSL
    };
    std::vector<UniformInfo> m_uniforms;
    
    enum Cullface {
        Cullface_Back = GL_BACK,
        Cullface_Front = GL_FRONT,
        Cullface_Off = GL_FRONT_AND_BACK,
    };
    
    Cullface m_cullface = Cullface_Back;
    bool m_ZWrite = true;
    
    friend class RenderSystem;
    static void staticInit();
    static std::map<std::string, PShader> m_builtinShaders;
};

#endif

