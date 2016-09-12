#include "Camera.hpp"
#include "Scene.hpp"

std::shared_ptr<Camera> Camera::main()
{
    return Scene::mainCamera();
}
