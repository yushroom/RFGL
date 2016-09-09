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
	Shader(Shader&&);

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
    
    
    void bindUniformTexture(const char* name, const GLuint texture, const GLuint id, GLenum textureType = GL_TEXTURE_2D);

private:
	GLuint m_program = 0;
    
    GLint _getUniformLocation(const char* name) const;
};

#endif

