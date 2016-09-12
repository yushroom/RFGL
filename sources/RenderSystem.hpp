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
	double m_time = 0.;
	int m_width;
	int m_height;
    
    std::vector<std::shared_ptr<IRunable>> m_runables;

public:

	static RenderSystem& getInstance();

	virtual void init() override;

	virtual void run() override;

	virtual void clean() override;

	void addRunable(std::shared_ptr<IRunable> p_runable) {
		m_runables.push_back(p_runable);
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

	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	static void windowSizeCallback(GLFWwindow* window, int width, int height);

	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

    bool m_isWireFrameMode = false;
};

#endif // RenderSystem_hpp