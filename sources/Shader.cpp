#include "Shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
using namespace std;

void Shader::FromString(const std::string &vs_string, const std::string &ps_string)
{
    FromString(vs_string, "", "", "", ps_string);
}

void Shader::FromString(const std::string& vs_string, const std::string& ps_string, const std::string& gs_string)
{
    FromString(vs_string, "", "", gs_string, ps_string);
}

void Shader::FromString(const std::string& vs_string,
                        const std::string& tcs_string,
                        const std::string& tes_string,
                        const std::string& gs_string,
                        const std::string& ps_string)
{
    assert(m_program == 0);
    assert(!vs_string.empty() && !ps_string.empty());
    //assert(tcs_string.empty() || (!tcs_string.empty() && !tes_string.empty()));
    assert(!(!tcs_string.empty() && tes_string.empty()));
    
    bool use_gs = !gs_string.empty();
    bool use_ts = !tes_string.empty();
    GLuint vs = 0;
    GLuint ps = 0;
    GLuint gs = 0;
    GLuint tcs = 0;
    GLuint tes = 0;
    
    auto add_line_number = [](const string& str) -> std::string {
        stringstream ss;
        int line_number = 2;
        ss << "#1 ";
        string::size_type last_pos = 0;
        auto pos = str.find('\n');
        while (pos != string::npos) {
            ss << str.substr(last_pos, pos-last_pos) << "\n#" <<line_number << " ";
            last_pos = pos+1;
            pos = str.find('\n', last_pos);
            line_number ++;
        };
        return ss.str();
    };
    
    auto compileShader = [&add_line_number](GLuint& shader, GLenum shader_type, const std::string& shader_str) {
        const GLchar* shader_c_str = shader_str.c_str();
        shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, &shader_c_str, NULL);
        glCompileShader(shader);
        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            std::cout << add_line_number(shader_str) << endl;
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << infoLog << endl;
            abort();
        }
    };
    
    compileShader(vs, GL_VERTEX_SHADER, ShaderMacro + vs_string);
    compileShader(ps, GL_FRAGMENT_SHADER, ShaderMacro + ps_string);
    
    // gs
    if (use_gs) {
        compileShader(gs, GL_GEOMETRY_SHADER, ShaderMacro + gs_string);
    }
    if (use_ts) {
        if (!tcs_string.empty())
            compileShader(tcs, GL_TESS_CONTROL_SHADER, ShaderMacro + tcs_string);
        compileShader(tes, GL_TESS_EVALUATION_SHADER, ShaderMacro + tes_string);
    }
    
    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, ps);
    if (use_gs)
        glAttachShader(m_program, gs);
    if (use_ts) {
        if (tcs != 0) glAttachShader(m_program, tcs);
        glAttachShader(m_program, tes);
    }
    glLinkProgram(m_program);
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_program, 1024, NULL, infoLog);
        std::cout << infoLog << endl;
        abort();
    }
    glDeleteProgram(vs);
    glDeleteProgram(ps);
    if (use_gs)
        glDeleteProgram(gs);
}

void Shader::FromFile(const std::string& vs_path, const std::string ps_path)
{
    assert(m_program == 0);
	std::ifstream vs_stream(vs_path);
	std::ifstream ps_stream(ps_path);
	assert(vs_stream.is_open());
	assert(ps_stream.is_open());
	std::stringstream vs_sstream, ps_sstream;
	vs_sstream << vs_stream.rdbuf();
	ps_sstream << ps_stream.rdbuf();
	string vs_string = vs_sstream.str();
	string ps_string = ps_sstream.str();
//	std::cout << vs_string << endl;
//	std::cout << ps_string << endl;
    FromString(vs_string, ps_string);
}


Shader::~Shader()
{
	glDeleteProgram(m_program);
}

void Shader::Use() {
    glUseProgram(this->m_program);
}

GLuint Shader::GetAttribLocation(const char* name) const {
    return glGetAttribLocation(m_program, name);
}

void Shader::BindUniformFloat(const char* name, const float value) const {
    GLint loc = GetUniformLocation(name);
    glUniform1f(loc, value);
}

void Shader::BindUniformVec3(const char* name, const glm::vec3& value) const {
    GLint loc = GetUniformLocation(name);
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Shader::BindUniformMat4(const char* name, const glm::mat4& value) const {
    GLint loc = GetUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::BindUniformMat3(const char* name, const glm::mat3& value) const {
    GLint loc = GetUniformLocation(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::BindUniformTexture(const char* name, const GLuint texture, const GLuint id, GLenum textureType /*= GL_TEXTURE_2D*/) {
    glActiveTexture(GLenum(GL_TEXTURE0+id));
    glBindTexture(textureType, texture);
    GLuint loc = GetUniformLocation(name);
    glUniform1i(loc, id);
}

GLint Shader::GetUniformLocation(const char* name) const {
    GLint loc = glGetUniformLocation(m_program, name);
    if (loc == -1) {
        printf("uniform[%s] not found\n", name);
    }
    return loc;
}