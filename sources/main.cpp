#include <iostream>
#include <map>

#include "RenderSystem.hpp"
#include "Debug.hpp"

#include "App.hpp"
#include "Input.hpp"
#include "GUI.hpp"
#include "Camera.hpp"
#include "Time.hpp"
#include "MeshFilter.hpp"
#include "MeshRenderer.hpp"

using namespace std;

typedef shared_ptr<Texture> PTexture;

class VisualizeNormal : public Script
{
public:
    bool visualizeNormal = true;
    shared_ptr<MeshRenderer> meshRenderer = nullptr;
    Material::PMaterial material = nullptr;
    
    virtual void start() override {
        GUI::addBool("VisiualizeNormal", visualizeNormal);
        meshRenderer = m_gameObject->getComponent<MeshRenderer>();
        material = Material::builtinMaterial("VisualizeNormal");
    }
    
    virtual void update() override {
        auto& materials = meshRenderer->materials();
        if (visualizeNormal) {
            bool already_in = false;
            for (auto& m : materials) {
                if (m == material) {
                    already_in = true;
                }
            }
            if (!already_in) {
                meshRenderer->addMaterial(material);
            }
        } else {
            if (materials[materials.size()-1] == material)
                materials.pop_back();
        }
    }
};

class ExampleApp1 : public App
{
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

        auto headModel = Model::createFromObjFile(models_dir + "/head/head_combined.obj", VertexUsagePNUT);
        
        auto sphere = Model::createFromObjFile(models_dir+"/Sphere.obj", VertexUsagePNUT);

		m_skyTexture.fromFile(textures_dir + "StPeters/DiffuseMap.dds");
		m_headDiffuseTexture.fromFile(models_dir + "/head/lambertian.jpg");
		m_headNormalMapTexture.fromFile(models_dir + "/head/NormalMap_RG16f_1024_mipmaps.dds");
        
        map<string, GLuint> textures;
        textures["skyTex"] = m_skyTexture.getGLTexuture();
        
        auto skyboxGO = Scene::createGameOjbect();
        skyboxGO->transform()->setScale(20, 20, 20);
        auto meshFilter = make_shared<MeshFilter>(sphere);
        auto material = Material::builtinMaterial("SkyBox");
        material->preBindAllTexture(textures);
        auto meshRenderer = make_shared<MeshRenderer>(material);
        skyboxGO->addComponent(meshFilter);
        skyboxGO->addComponent(meshRenderer);
        
        textures.clear();
        textures["diffuseMap"] = m_headDiffuseTexture.getGLTexuture();
        textures["normalMap"] = m_headNormalMapTexture.getGLTexuture();
        //m_normalMapShader->preBindAllTexture(textures);

        auto headGO = Scene::createGameOjbect();
        headGO->transform()->setScale(10, 10, 10);
        auto meshFilter1 = make_shared<MeshFilter>(headModel);
        auto material1 = Material::builtinMaterial("NormalMap");
        material1->preBindAllTexture(textures);
        auto meshRenderer1 = make_shared<MeshRenderer>(material1);
        //auto material2 = Material::builtinMaterial("VisualizeNormal");
        //meshRenderer1->addMaterial(material2);
        headGO->addComponent(meshFilter1);
        headGO->addComponent(meshRenderer1);
        headGO->addScript(make_shared<VisualizeNormal>());
        
		GUI::addFloat("FPS", m_fps);
	}

	virtual void run() override {
		m_fps = 1.f / Time::getDeltaTime();

        if (Input::getKeyDown(Input::KeyCode_A)) {
            Debug::Log("A pressed");
		}
		if (Input::getKey(Input::KeyCode_A)) {
			Debug::Log("A held");
		}
		if (Input::getKeyUp(Input::KeyCode_A)) {
			Debug::Log("A released");
		}
	}

	virtual void clean() override {

	}
    
private:
    Texture m_skyTexture;
    Texture m_headDiffuseTexture;
    Texture m_headNormalMapTexture;
    bool m_visualizeNormal = false;
    float m_fps = 0;
};


int main()
{
    Debug::Init();
	auto& render_system = RenderSystem::getInstance();
	render_system.addRunable(make_shared<ExampleApp1>());
	render_system.init();
	render_system.run();
	render_system.clean();
    return 0;
}
