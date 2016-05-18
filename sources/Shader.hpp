//
//  Texture.hpp
//  PRT
//
//  Created by 俞云康 on 5/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef Shader_hpp
#define Shader_hpp

#include "PRT.hpp"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STRING_MACRO(x) x

#define PositionIndex 0
#define NormalIndex 1
#define UVIndex 2
#define TangentIndex 3

const static std::string ShaderMacro = R"(#version 410 core
#define PositionIndex 0
#define NormalIndex 1
#define UVIndex 2
#define TangentIndex 3
)";

class Shader
{
public:
    Shader() = default;
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    void FromString(const std::string& vs_str, const std::string& fs_str);
    void FromString(const std::string& vs_str, const std::string& fs_str, const std::string& gs_str);
    void FromString(const std::string& vs_str,
                    const std::string& tcs_str,
                    const std::string& tes_str,
                    const std::string& gs_str,
                    const std::string& fs_str);
    void FromFile(const std::string& vs_path, const std::string ps_path);
	//Shader(const std::string& vs_path, const std::string ps_path);
    ~Shader();
    
    void Use();
    
    GLuint GetGLProgram() const {
        return m_program;
    }
    
    GLuint GetAttribLocation(const char* name) const;
    
    void BindUniformFloat(const char* name, const float value) const;
    void BindUniformVec3(const char* name, const glm::vec3& value) const;
    void BindUniformMat3(const char* name, const glm::mat3& value) const;
    void BindUniformMat4(const char* name, const glm::mat4& value) const;
    
    
    void BindUniformTexture(const char* name, const GLuint texture, const GLuint id, GLenum textureType = GL_TEXTURE_2D);

private:
	GLuint m_program = 0;
    
    GLint GetUniformLocation(const char* name) const;
};

#endif

