#include <iostream>

#include "RenderSystem.hpp"
#include "Log.hpp"

#include "App.hpp"
#include "Input.hpp"
#include "GUI.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "Time.hpp"

using namespace std;

// Shaders
const std::string simpleVS = R"(
uniform mat4 mvp;
layout (location = PositionIndex) in vec3 position;
void main ()
{
    gl_Position = mvp * vec4(position, 1);
}
)";

const std::string simpleFS = R"(
out vec4 color;
void main()
{
    color = vec4(1, 0, 0, 1);
}
)";

const std::string normalMapVS = R"(//#version 410 core
layout (location = PositionIndex) in vec3 position;
layout (location = NormalIndex) in vec3 normal;
layout (location = TangentIndex) in vec3 tangent;
layout (location = UVIndex) in vec2 uv;
//uniform sampler2D bumpTex;
uniform mat4 mvp;
uniform mat4 model;
out VS_OUT {
    vec3 normal;
    vec3 tangent;
    vec2 uv;
} vs_out;

void main()
{
    vs_out.normal = mat3(model) * normal;
    vs_out.tangent = mat3(model) * tangent;
    vs_out.uv = uv;
    gl_Position = mvp * vec4(position, 1.0);
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
layout(location = 0) in vec3 position;
out vec3 uv;
uniform mat4 mvp;
void main()
{
    gl_Position = mvp * vec4(position, 1);
    uv = position.xyz;
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
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
void main() {
    gl_Position = proj * view * model * vec4(position, 1.0f);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(proj * vec4(normalMatrix*normal, 1.0)));
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

class Rotator : public Script
{
public:

	bool isRunning = false;
	float rotateSpeed = 40;
	float dragSpeed = 10;

	virtual void start() override
	{
		GUI::addBool("rotate", isRunning);
	}

    virtual void update() override
    {
		if (isRunning)
			gameObject->transform.rotateAround(Vector3(0, 0, 0), Vector3(0, 1, 0), 1);

		if (Input::getMouseButton(1)) {
			float h = rotateSpeed * Input::getAxis(Input::Axis_MouseX);
			float v = rotateSpeed * Input::getAxis(Input::Axis_MouseY);
			info("%lf,  %lf", h, v);
			transform->rotate(v, -h, 0);
			// fix "up" dirction
			transform->lookAt(transform->getForward() + transform->getPosition());
		}

		if (Input::getMouseButton(2)) {
			float x = dragSpeed * Input::getAxis(Input::Axis_MouseX);
			float y = dragSpeed * Input::getAxis(Input::Axis_MouseY);
			transform->translate(-x, -y, 0);
		}
    }
};

class ExampleApp1 : public App
{
private:
	Shader m_normalMapShader;
	Shader m_skyShader;
	Shader m_visualizeNormalShader;

	Texture m_skyTexture;
	Texture m_headDiffuseTexture;
	Texture m_headNormalMapTexture;

	Model m_headModel;

	bool m_isWireFrameMode = false;
	bool m_visualizeNormal = false;

	float m_fps = 0;

public:

	virtual void init() override {
		glCheckError();
#if defined(_WIN32)
		const std::string root_dir = "../";
#else
		const std::string root_dir = "/Users/yushroom/program/graphics/RFGL/";
#endif
		const std::string models_dir = root_dir + "models/";
		const std::string textures_dir = root_dir + "textures/";

		m_normalMapShader.fromString(normalMapVS, normalMapFS);
		glCheckError();
		m_skyShader.fromString(skyBoxVS, skyBoxFS);
		glCheckError();
		m_visualizeNormalShader.fromString(visualizeNormalVS, visualizeNormalFS, visualizeNormalGS);
		glCheckError();

		m_headModel.fromObjFile(models_dir + "/head/head_combined.obj", VertexUsagePNUT);
		glCheckError();

		Model& quad = Model::getQuad();
		quad.setVertexUsage(VertexUsagePNUT);

		m_skyTexture.fromFile(textures_dir + "StPeters/DiffuseMap.dds");
		m_headDiffuseTexture.fromFile(models_dir + "/head/lambertian.jpg");
		m_headNormalMapTexture.fromFile(models_dir + "/head/NormalMap_RG16f_1024_mipmaps.dds");

		GUI::addFloat("FPS", m_fps);
		GUI::addBool("WireFrame", m_isWireFrameMode);
		GUI::addBool("VisiualizeNormal", m_visualizeNormal);
        
        auto camera = RenderSystem::getInstance().getMainCamera();
        camera->gameObject->addScript(make_shared<Rotator>());
	}

	virtual void run() override {
		m_fps = 1.f / Time::getDeltaTime();
		if (m_isWireFrameMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		//glPatchParameteri(GL_PATCH_VERTICES, 3);

		glm::mat4 model = glm::scale(glm::mat4(), glm::vec3(10, 10, 10));
		auto camera = RenderSystem::getInstance().getMainCamera();
		auto view = camera->getViewMatrix();
		auto proj = camera->getProjectMatrix();
		auto mvp = proj * view * model;

		//glDisable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		auto sky_mvp = proj * view * glm::scale(glm::mat4(), glm::vec3(20, 20, 20));
		m_skyShader.use();
		m_skyShader.bindUniformMat4("mvp", sky_mvp);
		m_skyShader.bindUniformTexture("skyTex", m_skyTexture.getGLTexuture(), 0, GL_TEXTURE_CUBE_MAP);
		Model::getSphere().render(m_skyShader);
		//glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		m_normalMapShader.use();
		m_normalMapShader.bindUniformMat4("mvp", mvp);
		m_normalMapShader.bindUniformMat4("model", model);
		//shader.BindUniformTexture("bumpTex", bumpTexture.GetGLTexuture(), 0);
		m_normalMapShader.bindUniformTexture("diffuseMap", m_headDiffuseTexture.getGLTexuture(), 0);
		m_normalMapShader.bindUniformTexture("normalMap", m_headNormalMapTexture.getGLTexuture(), 1);
		m_headModel.render(m_normalMapShader);

		auto modelView = view;

		if (m_visualizeNormal) {
			m_visualizeNormalShader.use();
			m_visualizeNormalShader.bindUniformMat4("model", model);
			m_visualizeNormalShader.bindUniformMat4("view", view);
			m_visualizeNormalShader.bindUniformMat4("proj", proj);
			m_headModel.render(m_visualizeNormalShader);
		}

		if (Input::getKeyDown(Input::KeyCode_A)) {
			info("A pressed");
		}
		if (Input::getKey(Input::KeyCode_A)) {
			info("A held");
		}
		if (Input::getKeyUp(Input::KeyCode_A)) {
			info("A released");
		}

		if (m_isWireFrameMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	virtual void clean() override {

	}
};


int main()
{
	initLogSystem();
	auto& render_system = RenderSystem::getInstance();
	render_system.addRunable(make_shared<ExampleApp1>());
	render_system.init();
	render_system.run();
	render_system.clean();
    return 0;
}
