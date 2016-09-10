#include "RenderSystem.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Log.hpp"
#include "GLError.hpp"
#include "Input.hpp"
#include "GUI.hpp"
#include "Camera.hpp"
#include "Time.hpp"

// Window dimensions
const uint32_t WIDTH = 800, HEIGHT = 600;

std::shared_ptr<Camera> RenderSystem::m_mainCamera = nullptr;

void Do_Movement()
{
	// Camera controls
	//if(keys[GLFW_KEY_W])
	//    camera.ProcessKeyboard(FORWARD, deltaTime);
	//if(keys[GLFW_KEY_S])
	//    camera.ProcessKeyboard(BACKWARD, deltaTime);
	//if(keys[GLFW_KEY_A])
	//    camera.ProcessKeyboard(LEFT, deltaTime);
	//if(keys[GLFW_KEY_D])
	//    camera.ProcessKeyboard(RIGHT, deltaTime);
}


RenderSystem::RenderSystem()
{

}

RenderSystem& RenderSystem::getInstance()
{
	static RenderSystem instance;
	return instance;
}

void RenderSystem::init()
{
	Input::init();

	info("Starting GLFW context, OpenGL 4.1");
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	m_window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(m_window);

	// Set the required callback functions
	glfwSetKeyCallback(m_window, RenderSystem::keyCallBack);
	glfwSetCursorPosCallback(m_window, RenderSystem::mouseCallback);
    glfwSetScrollCallback(m_window, RenderSystem::mouseScrollCallback);


	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
	glCheckError();

	// Define the view port dimensions
	glfwGetFramebufferSize(m_window, &m_width, &m_height);
	glViewport(0, 0, m_width, m_height);

	glfwSetWindowSizeCallback(m_window, RenderSystem::windowSizeCallback);
	glfwSetMouseButtonCallback(m_window, RenderSystem::mouseButtonCallback);

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	info("Max supported patch vertices %d", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	m_mainCamera = std::make_shared<Camera>(60.0f, float(m_width) / m_height, 0.1f, 100.f);
	auto camera_go = std::make_shared<GameObject>();
	camera_go->addComponent(m_mainCamera);
	camera_go->transform.setPosition(0, 0, 5);
	camera_go->transform.lookAt(Vector3(0, 0, 0));
	m_gameObjects.push_back(camera_go);

	GUI::init();

	m_time = glfwGetTime();

	for (auto& r : m_runables) {
		r->init();
	}

	for (auto& go : m_gameObjects) {
		go->start();
	}
}

void RenderSystem::run()
{
	// Game loop
	while (!glfwWindowShouldClose(RenderSystem::m_window))
	{
		Input::update();
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		Input::updateMousePosition(float(xpos)/m_width, float(ypos)/m_height);
        
        for (auto& go : m_gameObjects) {
            go->update();
        }

		// Render
		// Clear the color buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto& r : m_runables) {
			r->run();
		}

		GUI::update();

		// Swap the screen buffers
		glfwSwapBuffers(m_window);

        double new_t = glfwGetTime();
        Time::m_deltaTime = float(new_t - m_time);
        m_time = new_t;
	}
}

void RenderSystem::clean()
{
	for (auto& r : m_runables) {
		r->clean();
	}

	GUI::clean();

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}

void RenderSystem::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if ((key >= GLFW_KEY_0 && key <= GLFW_KEY_9) || (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)) {
		Input::updateKeyState((Input::KeyCode)key, (Input::KeyState)action);
	}

	//TwEventKeyGLFW(key, action);
    GUI::onKey(key, action);
}

void RenderSystem::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    //camera.ProcessMouseScroll(yoffset);
    //TwEventMouseWheelGLFW(yoffset);
	bool handled = GUI::onMouseScroll(yoffset);
	if (handled)
		return;
	auto& t = m_mainCamera->gameObject->transform;
	t.setPosition(t.getPosition() - -0.2f*float(yoffset)*t.getForward());
}

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

void RenderSystem::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	//TwEventMouseButtonGLFW(button, action);
	if (GUI::onMouseButton(button, action))
		return;
	Input::updateMouseButtonState(button, action == GLFW_PRESS ? Input::MouseButtonState_Down : Input::MouseButtonState_Up);
}

void RenderSystem::windowSizeCallback(GLFWwindow* window, int width, int height)
{
	//glViewport(0, 0, width, height);
	GUI::onWindowSizeChanged(width, height);
}

void RenderSystem::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	//camera.ProcessMouseMovement(xoffset, yoffset);
	GUI::onMouse(xpos, ypos);
}
