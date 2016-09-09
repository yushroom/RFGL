#ifndef App_hpp
#define App_hpp

#include "RenderSystem.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "GLError.hpp"
#include "Camera.hpp"

class App : public IRunable
{
public:
	//virtual void init() = 0;
	//virtual void run() = 0;
	//virtual void clean();
	virtual void onMouse() {};
	virtual void onKey() {};
};

//class ExampleApp2 : public App
//{
//public:
//	virtual void init() override {
//#if defined(_WIN32)
//		const std::string root_dir = "../";
//#else
//		const std::string root_dir = "/Users/yushroom/program/graphics/RFGL/";
//#endif
//		//const std::string root_dir = "/Users/yushroom/Downloads/head2";
//		const std::string models_dir = root_dir + "models/";
//		const std::string textures_dir = root_dir + "textures/";
//		//const std::string models_dir = root_dir;
//		//const std::string textures_dir = root_dir;
//
//		Shader shader;
//		shader.fromString(normalMapVS, normalMapFS);
//		Shader skyShader;
//		skyShader.fromString(skyBoxVS, skyBoxFS);
//		Shader visualizeNormalShader;
//		visualizeNormalShader.fromString(visualizeNormalVS, visualizeNormalFS, visualizeNormalGS);
//		Shader simpleShader;
//		simpleShader.fromString(simpleVS, simpleFS);
//
//		Shader tessShader;
//		tessShader.fromString(tessVS, tessTCS, tessTES, "", tessFS);
//		//Model head("/Users/yushroom/program/graphics/SoftRenderer/Model/head/head.OBJ");
//		//Model head(models_dir+"head/head_optimized.obj");
//		Model head(models_dir + "/head/head_combined.obj", VertexUsagePNUT);
//		Model sphere88(models_dir + "/sphere88.obj", VertexUsagePNUT);
//		//head.SetVertexUsage(VertexUsagePNUT);
//		//head.SetVertexUsage(VertexUsagePNUT);
//		glCheckError();
//
//		Model& quad = Model::getQuad();
//		quad.setVertexUsage(VertexUsagePNUT);
//
//		Texture skyTex(textures_dir + "StPeters/DiffuseMap.dds");
//		//auto& skyTex = Texture::GetSimpleTexutreCubeMap();
//		//Texture diffuse(models_dir+"head/DiffuseMap_R8G8B8A8_1024_mipmaps.dds");
//		Texture diffuse(models_dir + "/head/lambertian.jpg");
//		//bump.png
//		//Texture bumpTexture(models_dir + "/head/bump.png");
//		Texture bumpTexture(models_dir + "/head/bump-lowRes.png");
//		Texture normalMap(models_dir + "/head/NormalMap_RG16f_1024_mipmaps.dds");
//		//Texture diffuse("/Users/yushroom/program/github/SeparableSSS/SeparableSSS/head/head_optimized.png");
//		Texture bricksDiffuse(textures_dir + "/bricks_diffuse.jpg");
//		Texture bricksDisplacementMap(textures_dir + "/bricks_displacementmap.png");
//
//		// Uncommenting this call will result in wireframe polygons.
//		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//		glEnable(GL_DEPTH_TEST);
//		glEnable(GL_CULL_FACE);
//		//glDisable(GL_CULL_FACE);
//		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//	}
//
//	virtual void run() override {
//		//if (wire == 1)
//		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		//else
//		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//		glPatchParameteri(GL_PATCH_VERTICES, 3);
//
//		static float angle = 0;
//		glm::mat4 model = glm::rotate(glm::scale(glm::mat4(), glm::vec3(10, 10, 10)), angle, glm::vec3(0, 1, 0));
//		//angle += 0.005f;
//		if (angle > M_PI * 2.0f) angle -= M_PI * 2.0f;
//		Camera& camera = RenderSystem::getInstance().getMainCamera();
//		int width = RenderSystem::getInstance().getWidth();
//		int height = RenderSystem::getInstance().getHeight();
//		auto view = camera.GetViewMatrix();
//		auto proj = glm::perspective(glm::radians(camera.Zoom), float(width) / height, 0.1f, 100.f);
//		auto mvp = proj * view * model;
//
//		//        glDisable(GL_CULL_FACE);
//		//        auto sky_mvp = proj * view * glm::scale(glm::mat4(), glm::vec3(10, 10, 10));
//		//        skyShader.Use();
//		//        skyShader.BindUniformMat4("mvp", sky_mvp);
//		//        skyShader.BindUniformTexture("skyTex", skyTex.GetGLTexuture(), 0, GL_TEXTURE_CUBE_MAP);
//		//        Model::GetSphere().Render(skyShader);
//		//        glEnable(GL_CULL_FACE);
//
//		//        shader.Use();
//		//        shader.BindUniformMat4("mvp", mvp);
//		//        shader.BindUniformMat4("model", model);
//		//        //shader.BindUniformTexture("bumpTex", bumpTexture.GetGLTexuture(), 0);
//		//        shader.BindUniformTexture("diffuseMap", diffuse.GetGLTexuture(), 0);
//		//        shader.BindUniformTexture("normalMap", normalMap.GetGLTexuture(), 1);
//		//        head.Render(shader);
//
//		//auto modelView = view * glm::rotate(glm::mat4(), -45.0f, glm::vec3(1, 0, 0));
//		//auto modelView = view * model;
//		auto modelView = view;
//		//tessShader.Use();
//		//tessShader.BindUniformMat4("uViewProjMatrix", proj * view);
//		//tessShader.BindUniformMat4("uModelMatrix", glm::mat4());
//		//tessShader.BindUniformMat3("uNormalMatrix", glm::mat3(modelView));
//		////tessShader.BindUniformVec3("LightPosition", glm::vec3(5, 5, 5));
//		//tessShader.BindUniformFloat("TessLevelInner", tessLevelInner);
//		//tessShader.BindUniformFloat("TessLevelOuter", tessLevelOuter);
//		//tessShader.BindUniformFloat("bumpiness", bumpiness);
//		//tessShader.BindUniformTexture("displacementMap", bricksDisplacementMap.GetGLTexuture(), 0);
//		//tessShader.BindUniformTexture("diffuseMap", bricksDiffuse.GetGLTexuture(), 1);
//		////Model::GetIcosahedron().RenderPatch(tessShader);
//		////Model::GetBox().RenderPatch(tessShader);
//		////head.RenderPatch(tessShader);
//		//sphere88.RenderPatch(tessShader);
//		////quad.RenderPatch(tessShader);
//
//		//        glCheckError();
//		//        simpleShader.Use();
//		//        simpleShader.BindUniformMat4("mvp", proj * view);
//		//        //Model::GetIcosahedron().Render(simpleShader);
//		//        //Model::GetQuad().Render(simpleShader);
//		//        Model::GetBox().Render(simpleShader);
//
//		//        visualizeNormalShader.Use();
//		//        visualizeNormalShader.BindUniformMat4("model", model);
//		//        visualizeNormalShader.BindUniformMat4("view", view);
//		//        visualizeNormalShader.BindUniformMat4("proj", proj);
//		//        head.Render(shader);
//	}
//
//	virtual void clean() override {
//
//	}
//};

#endif // App_hpp