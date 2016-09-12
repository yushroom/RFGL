//
//  Scene.cpp
//  PRT
//
//  Created by 俞云康 on 9/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#include "Scene.hpp"
#include "RenderSystem.hpp"
#include "MeshRenderer.hpp"

std::vector<std::shared_ptr<GameObject>> Scene::m_gameObjects;

std::shared_ptr<Camera> Scene::m_mainCamera = nullptr;

void Scene::init() {
    int width = RenderSystem::getInstance().getWidth();
    int height = RenderSystem::getInstance().getHeight();
    m_mainCamera = std::make_shared<Camera>(60.0f, float(width) / height, 0.1f, 100.f);
    auto camera_go = std::make_shared<GameObject>();
    camera_go->addScript(std::make_shared<CameraController>());
    camera_go->addComponent(m_mainCamera);
    camera_go->transform()->setPosition(0, 0, 5);
    camera_go->transform()->lookAt(Vector3(0, 0, 0));
    m_gameObjects.push_back(camera_go);
}

void Scene::start() {
    for (auto& go : m_gameObjects) {
        go->start();
    }
}

void Scene::update() {
    for (auto& go : m_gameObjects) {
        go->update();
        auto renderer = go->getComponent<MeshRenderer>();
        if (renderer != nullptr) {
            renderer->render();
        }
    }
}

