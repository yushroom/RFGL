#include "Scene.hpp"
#include "RenderSystem.hpp"
#include "MeshRenderer.hpp"

std::vector<std::shared_ptr<GameObject>> Scene::m_gameObjects;

std::shared_ptr<Camera> Scene::m_mainCamera = nullptr;

void Scene::Init() {
    int width = RenderSystem::GetInstance().width();
    int height = RenderSystem::GetInstance().height();
    m_mainCamera = std::make_shared<Camera>(60.0f, float(width) / height, 0.1f, 100.f);
    auto camera_go = std::make_shared<GameObject>();
    camera_go->AddScript(std::make_shared<CameraController>());
    camera_go->AddComponent(m_mainCamera);
    camera_go->transform()->setPosition(0, 0, 5);
    camera_go->transform()->LookAt(Vector3(0, 0, 0));
    m_gameObjects.push_back(camera_go);
}

void Scene::Start() {
    for (auto& go : m_gameObjects) {
        go->Start();
    }
}

void Scene::Update() {
    for (auto& go : m_gameObjects) {
        if (!go->activeInHierarchy()) continue;
        go->Update();
        auto renderer = go->GetComponent<MeshRenderer>();
        if (renderer != nullptr) {
            renderer->Render();
        }
    }
}

