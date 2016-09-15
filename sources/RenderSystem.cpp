#include "RenderSystem.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "Debug.hpp"
#include "GLError.hpp"
#include "Input.hpp"
#include "GUI.hpp"
#include "Camera.hpp"
#include "Time.hpp"
#include "MeshRenderer.hpp"
#include "Scene.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

// Window dimensions
const uint32_t WIDTH = 800, HEIGHT = 600;
RenderSystem::RenderSystem()
{

}

RenderSystem& RenderSystem::GetInstance()
{
    static RenderSystem instance;
    return instance;
}

static void GlfwErrorCallback(int error, const char* description)
{
    Debug::LogError("Error %d: %s\n", error, description);
}

void RenderSystem::Init()
{
    Input::Init();

    Debug::Log("Starting GLFW context, OpenGL 4.1");
    glfwSetErrorCallback(GlfwErrorCallback);
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "RenderFish", nullptr, nullptr);
    glfwMakeContextCurrent(m_window);

    // Set the required callback functions
    glfwSetKeyCallback(m_window, RenderSystem::KeyCallBack);
    glfwSetCursorPosCallback(m_window, RenderSystem::MouseCallback);
    glfwSetScrollCallback(m_window, RenderSystem::MouseScrollCallback);
    glfwSetCharCallback(m_window, RenderSystem::CharacterCallback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();
    glCheckError();

    // Define the view port dimensions
    glfwGetFramebufferSize(m_window, &m_width, &m_height);
    glViewport(0, 0, m_width, m_height);

    glfwSetWindowSizeCallback(m_window, RenderSystem::WindowSizeCallback);
    glfwSetMouseButtonCallback(m_window, RenderSystem::MouseButtonCallback);
    
    ImGui_ImplGlfwGL3_Init(m_window, false);

    //GLint MaxPatchVertices = 0;
    //glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
    //Debug::Log("Max supported patch vertices %d", MaxPatchVertices);
    //glPatchParameteri(GL_PATCH_VERTICES, 3);

    Shader::Init();
    Material::Init();
    
    GUI::Init();
    //GUI::AddBool("WireFrame", m_isWireFrameMode);
    //GUI::AddBool("GammaCorrection", m_useGammaCorrection);
    
    Scene::Init();

    m_time = glfwGetTime();

    for (auto& r : m_runables) {
        r->Init();
    }
}

void RenderSystem::Run()
{
    Scene::Start();

    // Game loop
    while (!glfwWindowShouldClose(RenderSystem::m_window))
    {
        Input::Update();
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        double xpos, ypos;
        glfwGetCursorPos(m_window, &xpos, &ypos);
        Input::UpdateMousePosition(float(xpos)/m_width, float(ypos)/m_height);
        ImGui_ImplGlfwGL3_NewFrame();
        
        Scene::Update();

        // Render
        // Clear the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
#ifdef GLM_FORCE_LEFT_HANDED
        glFrontFace(GL_CW);
#endif

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        
        if (m_isWireFrameMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (m_useGammaCorrection)
            glEnable(GL_FRAMEBUFFER_SRGB);
        else
            glDisable(GL_FRAMEBUFFER_SRGB);

        for (auto& r : m_runables) {
            r->Run();
        }

        Scene::Render();
        
        if (m_isWireFrameMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui::Begin("Inspector");
        if (ImGui::CollapsingHeader("Global Render Settings")) {
            ImGui::Checkbox("Wire Frame", &m_isWireFrameMode);
            ImGui::Checkbox("Gamma Correction", &m_useGammaCorrection);
        }
        //ImGui::Render();
        Scene::OnEditorGUI();
        ImGui::End(); // Inspector
        
        GUI::Update();

        // Swap the screen buffers
        glfwSwapBuffers(m_window);

        double new_t = glfwGetTime();
        Time::m_deltaTime = float(new_t - m_time);
        m_time = new_t;
    }
}

void RenderSystem::Clean()
{
    for (auto& r : m_runables) {
        r->Clean();
    }

    GUI::Clean();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
}

void RenderSystem::SaveScreenShot(const std::string& path)
{
    auto pixels = new uint8_t[3*m_width*m_height*2];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, pixels+ m_width*m_height * 3);
    // horizontally flip
    auto line_a = pixels;
    auto line_b = pixels+m_width*m_height*3*2-m_width*3;
    for (int l = 0; l < m_height; ++l) 
    {
        memcpy(line_a, line_b, m_width*3*sizeof(uint8_t));
        line_a += m_width * 3;
        line_b -= m_width * 3;
    }
    stbi_write_png(path.c_str(), m_width, m_height, 3, pixels, 0);
    delete[] pixels;
}

void RenderSystem::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if ((key >= GLFW_KEY_0 && key <= GLFW_KEY_9) || (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)) {
        Input::UpdateKeyState((Input::KeyCode)key, (Input::KeyState)action);
    }

    //TwEventKeyGLFW(key, action);
    //GUI::OnKey(key, action);
    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mode);
}

void RenderSystem::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
//    bool handled = GUI::OnMouseScroll(yoffset);
//    if (handled)
//        return;
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
    //auto& t = m_mainCamera->gameObject->transform;
    auto t = Scene::mainCamera()->transform();
    t->setLocalPosition(t->position() + 0.2f*float(yoffset)*t->forward());
}

//GLfloat lastX = 400, lastY = 300;
//bool firstMouse = true;

void RenderSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
//    if (GUI::OnMouseButton(button, action))
//        return;
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
    Input::UpdateMouseButtonState(button, action == GLFW_PRESS ? Input::MouseButtonState_Down : Input::MouseButtonState_Up);
}

void RenderSystem::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    //glViewport(0, 0, width, height);
    //GUI::OnWindowSizeChanged(width, height);
}

void RenderSystem::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    //GUI::OnMouse(xpos, ypos);
}

void RenderSystem::CharacterCallback(GLFWwindow* window, unsigned int codepoint)
{
    ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
}
