#include <iostream>
#include <map>
#include <sstream>

#include "RenderSystem.hpp"
#include "Debug.hpp"

#include "App.hpp"
#include "Input.hpp"
#include "EditorGUI.hpp"
#include "Camera.hpp"
#include "Time.hpp"
#include "MeshFilter.hpp"
#include "MeshRenderer.hpp"

using namespace std;

class ShowFPS : public Script
{
public:
    InjectClassName(ShowFPS);

    float m_fps = 0;

    virtual void Start() override {
        //GUI::AddFloatRO("FPS", m_fps);
    }
    
    virtual void OnEditorGUI() override {
        m_fps = 1.f / Time::DeltaTime();
        ImGui::Text("FPS: %lf", m_fps);
    }

    virtual void Update() override {
        if (Input::GetKeyDown(Input::KeyCode_A)) {
            Debug::Log("A pressed");
        }
        if (Input::GetKey(Input::KeyCode_A)) {
            Debug::Log("A held");
        }
        if (Input::GetKeyUp(Input::KeyCode_A)) {
            Debug::Log("A released");
        }
    }
};

class DeactiveSelf : public Script
{
public:
    InjectClassName(DeactiveSelf);

    bool m_active = true;
    
    virtual void Start() override {
        //GUI::AddBool("show", m_active);
    }
    
    virtual void OnEditorGUI() override {
        ImGui::Checkbox("show", &m_active);
    }

    virtual void Update() override {
        if (m_active && !m_gameObject->activeSelf()) {
            Debug::Log("show");
            m_gameObject->SetActive(true);
        }
        if (!m_active && m_gameObject->activeSelf()) {
            Debug::Log("hide");
            m_gameObject->SetActive(false);
        }
    }

};

class VisualizeNormal : public Script
{
private:
    bool m_added = false;
    shared_ptr<MeshRenderer> m_meshRenderer = nullptr;
    Material::PMaterial m_material = nullptr;

public:
    InjectClassName(VisualizeNormal);

    bool m_visualizeNormal = false;

    virtual void Start() override {
        //GUI::AddBool("VisiualizeNormal", m_visualizeNormal);
        m_meshRenderer = m_gameObject->GetComponent<MeshRenderer>();
        m_material = Material::builtinMaterial("VisualizeNormal");
    }
    
    virtual void OnEditorGUI() override {
        ImGui::Checkbox("Visiualize Normal", &m_visualizeNormal);
    }
    
    virtual void Update() override {
        auto& materials = m_meshRenderer->materials();
        if (m_visualizeNormal) {
            if (!m_added) {
                m_meshRenderer->AddMaterial(m_material);
                m_added = true;
            }
        } else {
            if (materials[materials.size()-1] == m_material) {
                materials.pop_back();
                m_added = false;
            }
        }
    }
};

class TakeScreenShot : public Script
{
public:
    InjectClassName(TakeScreenShot);
    
//    virtual void Update() override {
//        if (Input::GetKeyDown(Input::KeyCode_P)) {
//        }
//    }
    
    virtual void OnEditorGUI() override {
        if (ImGui::Button("Screen shot")) {
            auto tm = time(nullptr);
            ostringstream ss;
            ss << int(tm) << ".png";
            RenderSystem::GetInstance().SaveScreenShot(ss.str());
            Debug::Log("Screen shot saved to %s", ss.str().c_str());

        }
    }
};

class ExampleApp1 : public App
{
public:

    virtual void Init() override {
        glCheckError();
#if defined(_WIN32)
        const std::string root_dir = "../";
#else
        const std::string root_dir = "/Users/yushroom/program/graphics/RFGL/";
#endif
        const std::string models_dir = root_dir + "models/";
        const std::string textures_dir = root_dir + "textures/";

        auto headModel = Mesh::CreateFromObjFile(models_dir + "/head/head_combined.obj", VertexUsagePNUT);
        
        auto sphere = Mesh::CreateFromObjFile(models_dir+"/Sphere.obj", VertexUsagePNUT);

        auto sky_texture = Texture::CreateFromFile(textures_dir + "StPeters/DiffuseMap.dds");
        auto head_diffuse = Texture::CreateFromFile(models_dir + "/head/lambertian.jpg");
        auto head_normalmap = Texture::CreateFromFile(models_dir + "/head/NormalMap_RG16f_1024_mipmaps.dds");
        
        map<string, Texture::PTexture> textures;
        textures["skyTex"] = sky_texture;
        
        auto skyboxGO = Scene::CreateGameObject("SkyBox");
        skyboxGO->transform()->setLocalScale(20, 20, 20);
        auto meshFilter = make_shared<MeshFilter>(sphere);
        auto material = Material::builtinMaterial("SkyBox");
        material->BindTextures(textures);
        auto meshRenderer = make_shared<MeshRenderer>(material);
        skyboxGO->AddComponent(meshFilter);
        skyboxGO->AddComponent(meshRenderer);
        
        //textures.clear();
        //textures["diffuseMap"] = head_diffuse;
        //textures["normalMap"] = head_normalmap;

//        auto headGO = Scene::CreateGameObject();
//        headGO->transform()->setScale(10, 10, 10);
//        auto meshFilter1 = make_shared<MeshFilter>(headModel);
//        auto material1 = Material::builtinMaterial("NormalMap");
//        material1->BindTextures(textures);
//        auto meshRenderer1 = make_shared<MeshRenderer>(material1);
//        headGO->AddComponent(meshFilter1);
//        headGO->AddComponent(meshRenderer1);
//        headGO->AddScript(make_shared<VisualizeNormal>());
//        //headGO->AddScript(make_shared<DeactiveSelf>());
        textures["AmbientCubemap"] = sky_texture;
        auto go = Scene::CreateGameObject("Sphere");
        //go->transform()->setScale(20, 20, 20);
        meshFilter = make_shared<MeshFilter>(sphere);
        material = Material::builtinMaterial("PBR");
        material->SetVector3("albedo", Vector3(0.8f, 0.6f, 0.6f));
        material->BindTextures(textures);
        meshRenderer = make_shared<MeshRenderer>(material);
        go->AddComponent(meshFilter);
        go->AddComponent(meshRenderer);
        //go->AddScript(make_shared<DeactiveSelf>());
        Scene::mainCamera()->gameObject()->AddScript(make_shared<ShowFPS>());
        Scene::mainCamera()->gameObject()->AddScript(make_shared<TakeScreenShot>());
        
        auto child0 = Scene::CreateGameObject("child0");
        child0->transform()->setParent(go->transform());
        auto child1 = Scene::CreateGameObject("child1");
        child1->transform()->setParent(go->transform());
        auto child2 = Scene::CreateGameObject("child2");
        child2->transform()->setParent(go->transform());
        
        auto child3 = Scene::CreateGameObject("child3");
        child3->transform()->setParent(child0->transform());
        
        //Scene::SelectGameObject(go.get());
    }

    virtual void Run() override {
    }

    virtual void Clean() override {
    }
};


int main()
{
    Debug::Init();
    auto& render_system = RenderSystem::GetInstance();
    render_system.AddRunable(make_shared<ExampleApp1>());
    render_system.Init();
    render_system.Run();
    render_system.Clean();
    return 0;
}
