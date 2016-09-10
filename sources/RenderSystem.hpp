#ifndef RenderSystem_hpp
#define RenderSystem_hpp

#include "RFGL.hpp"
#include <vector>
#include <memory>

#include "IRunable.hpp"
#include "Camera.hpp"

class RenderSystem : public IRunable
{
private:
	RenderSystem();

public:
	GLFWwindow* m_window;
	double m_fps = 0;
	double m_time = 0.;
	int m_width;
	int m_height;
	std::vector<std::shared_ptr<GameObject>> m_gameObjects;
	std::shared_ptr<Camera> m_mainCamera;

	std::vector<std::shared_ptr<IRunable>> m_runables;

public:

	static RenderSystem& getInstance();

	virtual void init() override;

	virtual void run() override;

	virtual void clean() override;

	void addRunable(std::shared_ptr<IRunable> p_runable) {
		m_runables.push_back(p_runable);
	}

	std::shared_ptr<Camera> getMainCamera() {
		return m_mainCamera;
	}

	int getWidth() {
		return m_width;
	}

	int getHeight() {
		return m_height;
	}

private:
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
    
    static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif // RenderSystem_hpp