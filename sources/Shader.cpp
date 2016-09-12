#include "Shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>

#include "GLError.hpp"
#include "Texture.hpp"

using namespace std;

Shader::Shader(Shader&& s)
{
    m_program = s.m_program;
}

void Shader::FromString(const std::string &vs_string, const std::string &fs_string)
{
    FromString(vs_string, "", "", "", fs_string);
}

void Shader::FromString(const std::string& vs_string, const std::string& fs_string, const std::string& gs_string)
{
    FromString(vs_string, "", "", gs_string, fs_string);
}


string GLenumToString(GLenum e) {
    switch (e) {
        case GL_FLOAT:
            return "GL_FLOAT";
        case GL_FLOAT_VEC3:
            return "GL_FLOAT_VEC3";
        case GL_FLOAT_MAT4:
            return "GL_FLOAT_MAT4";
        case GL_SAMPLER_2D:
            return "GL_SAMPLER_2D";
        case GL_SAMPLER_3D:
            return "GL_SAMPLER_3D";
        case GL_SAMPLER_CUBE:
            return "GL_SAMPLER_CUBE";
        default:
            return "UNKNOWN";
            break;
    }
}

std::vector<std::string> split(const std::string& str, const std::string& separator){
    auto l = separator.length();
    auto pos = str.find_first_of(separator);
    decltype(pos) last_pos = 0;
    std::vector<std::string> ret;
    while (pos != string::npos) {
        ret.push_back(str.substr(last_pos, pos-last_pos));
        pos += l;
        last_pos = pos;
        pos = str.find_first_of(separator, pos);
    }
    ret.push_back(str.substr(last_pos, pos));
    return ret;
}

void trim(std::string& str) {
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ')+1);
}

bool startsWith(const std::string& str, const std::string& str2) {
    return str.substr(0, str2.size()) == str2;
}

void Shader::FromString(const std::string& vs_string,
                        const std::string& tcs_string,
                        const std::string& tes_string,
                        const std::string& gs_string,
                        const std::string& fs_string)
{
    assert(m_program == 0);
    assert(!vs_string.empty() && !fs_string.empty());
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
        GLint success = GL_FALSE;
        //GLchar infoLog[1024];
        GLint infoLogLength = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (!success) {
            std::vector<char> infoLog(infoLogLength + 1);
            std::cout << add_line_number(shader_str) << endl;
            glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog.data());
            std::cout << string(&infoLog[0]) << endl;
            abort();
        }
    };
    
    map<string, string> settings= {{"Cull", "Back"},{"ZWrite", "On"}};
    
    auto extractSettings = [&settings](const string& shader_str) -> void {
        auto lines = split(shader_str, "\n");
        for (auto& line : lines) {
            if (startsWith(line, "///")) {
                line = line.substr(3);
                trim(line);
                auto s = split(line, " ");
                if (s.size() > 2) {
                    Debug::LogWarning("Incorrect shader setting format: %s", line.c_str());
                }
                auto res = settings.find(s[0]);
                if (res != settings.end()) {
                    Debug::Log("Override shader setting: %s", line.c_str());
                    settings[s[0]] = s[1];
                } else {
                    Debug::LogWarning("Unkown shader setting: %s", line.c_str());
                }
            }
        }
    };
    
    extractSettings(vs_string);
    if (settings["Cull"] == "Back") {
        m_cullface = Cullface_Back;
    } else if ((settings["Cull"] == "Front")) {
        m_cullface = Cullface_Front;
    } else {
        m_cullface = Cullface_Off;
    }
    m_ZWrite = settings["ZWrite"] == "On";
    
    compileShader(vs, GL_VERTEX_SHADER, ShaderMacro + vs_string);
    compileShader(ps, GL_FRAGMENT_SHADER, ShaderMacro + fs_string);
    
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
    
    GLint count;
    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)
    const GLsizei bufSize = 32; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length
    
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &count);
    Debug::Log("Program %u, Active Uniforms: %d", m_program, count);
    m_uniforms.clear();
    
    for (int i = 0; i < count; i++)
    {
        glGetActiveUniform(m_program, (GLuint)i, bufSize, &length, &size, &type, name);
        //GLint location = glGetUniformLocation(m_program, name);
        Debug::Log("Uniform #%d Type: %s Name: %s", i, GLenumToString(type).c_str(), name);
        m_uniforms.push_back(UniformInfo{type, string(name), (GLuint)i});
    }

    glDetachShader(m_program, vs);
    glDetachShader(m_program, ps);
    if (use_gs) {
        glDetachShader(m_program, gs);
    }
    if (use_ts) {
        if (tcs != 0) glDetachShader(m_program, tcs);
        glDetachShader(m_program, tes);
    }
    glDeleteShader(vs);
    glDeleteShader(ps);
    if (use_gs) {
        glDeleteShader(gs);
    }
    if (use_ts) {
        if (tcs != 0) glDeleteShader(tcs);
        glDeleteShader(tes);
    }
    glCheckError();
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
    string fs_string = ps_sstream.str();
//	std::cout << vs_string << endl;
//	std::cout << fs_string << endl;
    FromString(vs_string, fs_string);
}


Shader::~Shader()
{
    glDeleteProgram(m_program);
}

void Shader::Use() const {
    glUseProgram(this->m_program);
}

//GLuint Shader::getAttribLocation(const char* name) const {
//    return glGetAttribLocation(m_program, name);
//}

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

void Shader::BindUniformTexture(const char* name, const GLuint texture, const GLuint id, GLenum textureType /*= GL_TEXTURE_2D*/) const {
    glActiveTexture(GLenum(GL_TEXTURE0+id));
    glBindTexture(textureType, texture);
    GLuint loc = GetUniformLocation(name);
    glUniform1i(loc, id);
}

void Shader::BindBuiltinUniforms(const BuiltinShaderUniforms& uniforms) const
{
    for (auto& u : m_uniforms) {
        if (u.type == GL_FLOAT_MAT4) {
            auto it = uniforms.mat4s.find(u.name);
            if (it != uniforms.mat4s.end()) {
                //BindUniformMat4(u.name.c_str(), it->second);
                glUniformMatrix4fv(u.location, 1, GL_FALSE, glm::value_ptr(it->second));
            }
            else {
                Debug::LogWarning("%s of type %u not found", u.name.c_str(), u.type);
            }
        }
        else if (u.type == GL_FLOAT_VEC3) {
            auto it = uniforms.vec3s.find(u.name);
            if (it != uniforms.vec3s.end()) {
                //BindUniformVec3(u.name.c_str(), it->second);
                glUniform3fv(u.location, 1, glm::value_ptr(it->second));
            }
            else {
                Debug::LogWarning("%s of type %u not found", u.name.c_str(), u.type);
            }
        }
    }
}

void Shader::BindTextures(const std::map<std::string, Texture::PTexture>& textures) const
{
    int texture_id = 0;
    for (auto& u : m_uniforms) {
        if (u.type != GL_SAMPLER_2D && u.type != GL_SAMPLER_CUBE) continue;
        auto it = textures.find(u.name);
        if (it != textures.end()) {
            GLenum type = u.type == GL_SAMPLER_2D ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP;
            //BindUniformTexture(u.name.c_str(), it->second, texture_id, type);
            glActiveTexture(GLenum(GL_TEXTURE0 + texture_id));
            glBindTexture(type, it->second->GLTexuture());
            //GLuint loc = _getUniformLocation(name);
            glUniform1i(u.location, texture_id);
            texture_id++;
        }
        else {
            Debug::LogWarning("%s of type %u not found", u.name.c_str(), u.type);
        }
    }
}

void Shader::PreRender() const
{
    glCullFace(m_cullface);
    glDepthMask(m_ZWrite);
}

void Shader::PostRender() const
{
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
}

Shader::PShader Shader::builtinShader(const std::string& name)
{
    auto it = m_builtinShaders.find(name);
    if (it != m_builtinShaders.end()) {
        return it->second;
    }
    Debug::LogWarning("No builtin shader called %d", name.c_str());
    return nullptr;
}

GLint Shader::GetUniformLocation(const char* name) const {
    GLint loc = glGetUniformLocation(m_program, name);
    if (loc == -1) {
        printf("uniform[%s] not found\n", name);
    }
    return loc;
}


//========== Static Region ==========

// Built-in shader string
const std::string simpleVS = R"(
layout (location = PositionIndex) in vec3 position;
void main ()
{
    gl_Position = MATRIX_MVP * vec4(position, 1);
}
)";

const std::string simpleFS = R"(
out vec4 color;
void main()
{
    color = vec4(1, 0, 0, 1);
}
)";

const std::string tessVS = R"(
layout (location = PositionIndex) in vec3 position;
layout (location = NormalIndex) in vec3 normal;
layout (location = UVIndex) in vec2 uv;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;

void main() {
    vs_out.position = vec3( uModelMatrix * vec4(position, 1) );
    vs_out.normal = uNormalMatrix * normal;
    vs_out.uv = uv;
}
)";

const std::string tessTCS = R"(
layout(vertices = 3) out;
in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out[];
out TCS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} tcs_out[];
//out vec3 tcPosition[];
uniform float TessLevelInner;
uniform float TessLevelOuter;

#define ID gl_InvocationID

void main()
{
    //tcPosition[ID] = vs_out[ID].position;
    tcs_out[ID].position = vs_out[ID].position;
    tcs_out[ID].normal = vs_out[ID].normal;
    tcs_out[ID].uv = vs_out[ID].uv;
    
    //if (ID == 0) {
    gl_TessLevelInner[0] = TessLevelInner;
    gl_TessLevelOuter[0] = TessLevelOuter;
    gl_TessLevelOuter[1] = TessLevelOuter;
    gl_TessLevelOuter[2] = TessLevelOuter;
    //}
}
)";

// phong tessellation
const std::string tessTES = R"(
layout(triangles, equal_spacing, ccw) in;
//in vec3 tcPosition[];
in TCS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} tcs_out[];
//out vec3 tePosition;
//out vec3 tePatchDistance;
out TES_OUT {
    vec3 position;
    vec3 normal;
    //vec3 patchDistance;
    vec2 uv;
} tes_out;
uniform mat4 uViewProjMatrix;
//uniform mat4 Projection;
//uniform mat4 Modelview;
//uniform mat3 NormalMatrix;
uniform sampler2D displacementMap;
uniform float bumpiness;
const float alpha = 0.75;   // phong tessellation control factor

vec3 Project(vec3 q, vec3 p, vec3 n) {
    return q - dot(q-p, n)*n;
}

void main()
{
    //tes_out.patchDistance = gl_TessCoord;
    vec3 linear_pos = mat3(tcs_out[0].position, tcs_out[1].position, tcs_out[2].position) * gl_TessCoord;
    vec3 proj_pos = mat3(Project(linear_pos, tcs_out[0].position, tcs_out[0].normal),
                         Project(linear_pos, tcs_out[1].position, tcs_out[1].normal),
                         Project(linear_pos, tcs_out[2].position, tcs_out[2].normal))
    * gl_TessCoord;
    tes_out.position = mix(linear_pos, proj_pos, alpha);
    tes_out.normal = mat3(tcs_out[0].normal,
                          tcs_out[1].normal,
                          tcs_out[2].normal) * gl_TessCoord;
    tes_out.normal = normalize(tes_out.normal);
    tes_out.uv = gl_TessCoord.x * tcs_out[0].uv +
    gl_TessCoord.y * tcs_out[1].uv +
    gl_TessCoord.z * tcs_out[2].uv;
    tes_out.position += texture(displacementMap, tes_out.uv).r * bumpiness * tes_out.normal;
    gl_Position =  uViewProjMatrix * vec4(tes_out.position, 1);
}
)";

//const std::string tessGS = R"(
//uniform mat4 Modelview;
//uniform mat3 NormalMatrix;
//layout(triangles) in;
//layout(triangle_strip, max_vertices = 3) out;
//in vec3 tePosition[3];
//in vec3 tePatchDistance[3];
//out vec3 gFacetNormal;
//out vec3 gPatchDistance;
//out vec3 gTriDistance;
//
//void main()
//{
//    vec3 A = tePosition[2] - tePosition[0];
//    vec3 B = tePosition[1] - tePosition[0];
//    gFacetNormal = NormalMatrix * normalize(cross(A, B));
//
//    gPatchDistance = tePatchDistance[0];
//    gTriDistance = vec3(1, 0, 0);
//    gl_Position = gl_in[0].gl_Position; EmitVertex();
//
//    gPatchDistance = tePatchDistance[1];
//    gTriDistance = vec3(0, 1, 0);
//    gl_Position = gl_in[1].gl_Position; EmitVertex();
//
//    gPatchDistance = tePatchDistance[2];
//    gTriDistance = vec3(0, 0, 1);
//    gl_Position = gl_in[2].gl_Position; EmitVertex();
//
//    EndPrimitive();
//}
//)";

const std::string tessFS = R"(
out vec4 FragColor;
in TES_OUT {
    vec3 position;
    vec3 normal;
    //vec3 patchDistance;
    vec2 uv;
} tes_out;
in float gPrimitive;
//uniform vec3 LightPosition;
uniform sampler2D diffuseMap;
const vec3 lightDir = normalize(vec3(1, 1, 1));

void main()
{
    vec3 N = normalize(tes_out.normal);
    float NDotL = clamp(dot(N, lightDir), 0.0, 1.0);
    FragColor = texture(diffuseMap, tes_out.uv) * NDotL;
    //FragColor.rgb = vec3(NDotL);
    //FragColor.a = 1;
}
)";

const std::string visualizeNormalVS = R"(//#version 410 core
layout (location = PositionIndex) in vec3 position;
layout (location = NormalIndex) in vec3 normal;
out VS_OUT {
    vec3 normal;
} vs_out;
out vec3 fragNormal;

void main() {
    gl_Position = MATRIX_MVP * vec4(position, 1.0f);
    mat3 normalMatrix = mat3(MATRIX_IT_MV);
    vs_out.normal = normalize(vec3(MATRIX_P * vec4(normalMatrix*normal, 1.0)));
}
)";

const std::string visualizeNormalGS = R"(//#version 410 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.3f;

void GenerateLine(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.f) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}
)";

const std::string visualizeNormalFS = R"(//#version 330 core
out vec4 color;
void main()
{
    color = vec4(1.0, 0.5, 1.0, 1.0);
}
)";

std::map<std::string, Shader::PShader> Shader::m_builtinShaders;

const std::string normalMapVS = R"(//#version 410 core
layout (location = PositionIndex) in vec3 position;
layout (location = NormalIndex) in vec3 normal;
layout (location = TangentIndex) in vec3 tangent;
layout (location = UVIndex) in vec2 uv;
//uniform sampler2D bumpTex;

out VS_OUT {
    vec3 normal;
    vec3 tangent;
    vec2 uv;
} vs_out;

void main()
{
    vs_out.normal = mat3(_Object2World) * normal;
    vs_out.tangent = mat3(_Object2World) * tangent;
    vs_out.uv = uv;
    gl_Position = MATRIX_MVP * vec4(position, 1.0);
}
)";
const std::string normalMapFS = R"(//#version 410 core
const vec3 lightDir = normalize(vec3(1, 1, 1));
const float bumpiness = 0.2;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
in VS_OUT {
    vec3 normal;
    vec3 tangent;
    vec2 uv;
} vs_out;
out vec4 color;
void main()
{
    vec3 N = normalize(vs_out.normal);
    vec3 T = normalize(vs_out.tangent);
    vec3 B = normalize(cross(T, N));
    mat3 TBN = mat3(T, B, N);
    vec3 bump_normal;
    bump_normal.xy = 2.0 * texture(normalMap, vs_out.uv).gr - 1.0;
    bump_normal.z = sqrt(1.0 - dot(bump_normal.xy, bump_normal.xy));
    vec3 tangent_normal = mix(vec3(0, 0, 1), bump_normal, bumpiness);
    vec3 normal = TBN * tangent_normal;
    
    float c = clamp(dot(normal, lightDir), 0.0, 1.0);
    
    color.rgb = c * texture(diffuseMap, vs_out.uv).rgb;
    //color.rgb = vec3(c,c,c);
    color.a = 1.0f;
}
)";

const std::string skyBoxVS = R"(//#version 410 core
///Cull Front
///ZWrite Off
layout(location = 0) in vec3 position;
out vec3 uv;

void main()
{
    uv = position.xyz;
    vec3 p = (_Object2World * vec4(position, 1)).xyz + _WorldSpaceCameraPos;
    gl_Position = MATRIX_VP * vec4(p, 1);
    //uv.z = -uv.z;
}
)";

const std::string skyBoxFS = R"(//#version 410 core
in vec3 uv;
layout(location = PositionIndex) out vec4 color;
uniform float intensity;
uniform samplerCube skyTex;
void main()
{
    color = texture(skyTex, normalize(uv));
    //color.rgb = normalize(uv);
    color.a = 1.0;
}
)";

const std::string PBR_VS = R"(
#define PI 3.1415926f
vec3 diffuse(in vec3 diffuse) {
    return diffuse / PI;
}

CookTorrance

)";

void Shader::Init() {
    m_builtinShaders["NormalMap"] = Shader::CreateFromString(normalMapVS, normalMapFS);
    m_builtinShaders["SkyBox"] = Shader::CreateFromString(skyBoxVS, skyBoxFS);
    m_builtinShaders["VisualizeNormal"] = Shader::CreateFromString(visualizeNormalVS, visualizeNormalFS, visualizeNormalGS);
}