#ifndef RenderSystem_hpp
#define RenderSystem_hpp

#include "RFGL.hpp"
#include <vector>
#include <memory>

#include "IRunable.hpp"

class GameObject;

class RenderSystem
{
private:
    RenderSystem();

public:
    GLFWwindow* m_window;
    double m_time = 0.;
    int m_width;
    int m_height;
    
    std::vector<std::shared_ptr<IRunable>> m_runables;

public:

    static RenderSystem& GetInstance();

    void Init();

    void Run();

    void Clean();

    void AddRunable(std::shared_ptr<IRunable> p_runable) {
        m_runables.push_back(p_runable);
    }
    
    int width() {
        return m_width;
    }

    int height() {
        return m_height;
    }

    void SaveScreenShot(const std::string& path);

private:
    static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);

    static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    static void WindowSizeCallback(GLFWwindow* window, int width, int height);

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    
    static void CharacterCallback(GLFWwindow* window, unsigned int codepoint);

    bool m_isWireFrameMode = false;
    bool m_useGammaCorrection = true;
};

#endif // RenderSystem_hpp