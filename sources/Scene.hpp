//
//  Scene.hpp
//  PRT
//
//  Created by 俞云康 on 9/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef Scene_hpp
#define Scene_hpp

#include "GameObject.hpp"
#include "Camera.hpp"

class Scene
{
public:
    static std::shared_ptr<Camera> getMainCamera() {
        return m_mainCamera;
    }
    
    static std::shared_ptr<GameObject> createGameOjbect() {
        auto go = std::make_shared<GameObject>();
        m_gameObjects.push_back(go);
        return go;
    }
    
    static void init();
    static void start();
    static void update();
    
private:
    static std::shared_ptr<Camera> m_mainCamera;
    static std::vector<std::shared_ptr<GameObject>> m_gameObjects;
    
};

#endif /* Scene_hpp */
