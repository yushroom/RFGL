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

static std::vector<std::string> builtinUniformNames{
"MATRIX_MVP", "MATRIX_V", "MATRIX_P", "MATRIX_VP", "MATRIX_IT_MV",
    "_Object2World", "_WorldSpaceCameraPos"
};

const static std::string ShaderMacro = R"(#version 410 core
#define PositionIndex 0
#define NormalIndex 1
#define UVIndex 2
#define TangentIndex 3

#define PI 3.141592653589793f
#define INV_PI 0.3183098861837907f

// Transformations
uniform mat4 MATRIX_MVP;
//uniform mat4 MATRIX_MV;
uniform mat4 MATRIX_V;
uniform mat4 MATRIX_P;
uniform mat4 MATRIX_VP;
//uniform mat4 MATRIX_T_MV;
uniform mat4 MATRIX_IT_MV;
uniform mat4 MATRIX_IT_M;   // new
uniform mat4 _Object2World; // MATRIX_M
//uniform mat4 _World2Object; // MATRIX_I_M

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

// HLSL
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define uint2 uvec2
#define uint3 uvec3
#define uint4 uvec4
#define frac(x) fract(x)
#define saturate(x) clamp(x, 0.0, 1.0)
)";

struct ShaderUniforms
{
    std::map<std::string, Matrix4x4> mat4s;
    std::map<std::string, Vector3> vec3s;
    std::map<std::string, float> floats;
};

class RenderSystem;
class Texture;

class Shader
{
public:
    Shader() = default;
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&);

    typedef std::shared_ptr<Shader> PShader;

    static PShader CreateFromString(const std::string& vs_str, const std::string& fs_str) {
        auto s = std::make_shared<Shader>();
        s->FromString(vs_str, fs_str);
        return s;
    }
    
    static PShader CreateFromString(const std::string& vs_str, const std::string& fs_str, const std::string& gs_str) {
        auto s = std::make_shared<Shader>();
        s->FromString(vs_str, fs_str, gs_str);
        return s;
    }

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
    
    void Use() const;
    
    GLuint program() const {
        return m_program;
    }
    
    //GLuint getAttribLocation(const char* name) const;
    
    //void BindUniformFloat(const char* name, const float value) const;
    //void BindUniformVec3(const char* name, const glm::vec3& value) const;
    //void BindUniformMat3(const char* name, const glm::mat3& value) const;
    //void BindUniformMat4(const char* name, const glm::mat4& value) const;

    //void BindUniformTexture(const char* name, const GLuint texture, const GLuint id, GLenum textureType = GL_TEXTURE_2D) const;

    void BindUniforms(const ShaderUniforms& uniforms);
    void BindTextures(const std::map<std::string, std::shared_ptr<Texture>>& textures);
    
    void PreRender() const;
    void PostRender() const;
    
    void CheckStatus() const;

    static PShader builtinShader(const std::string& name);
    
private:
    GLuint m_program = 0;
    
    GLint GetUniformLocation(const char* name) const;
    
    struct UniformInfo {
        //GLint size; // size of the variable
        GLenum type; // type of the variable (float, vec3 or mat4, etc)
        //GLchar name[32];
        std::string name;  // variable name in GLSL
        GLuint location;
        bool binded;
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
    static void Init();
    static std::map<std::string, PShader> m_builtinShaders;
};

#endif

