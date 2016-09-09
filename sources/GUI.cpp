#include "GUI.hpp"
#include <AntTweakBar.h>
#include "RenderSystem.hpp"

TwBar* GUI::m_twBar = nullptr;

void GUI::init()
{
	TwInit(TW_OPENGL_CORE, NULL);
	m_twBar = TwNewBar("Tweak");
	TwWindowSize(RenderSystem::getInstance().getWidth(), RenderSystem::getInstance().getHeight());

	//static uint32_t tessLevelInner = 3;
	//static uint32_t tessLevelOuter = 2;
	//static float bumpiness = 0.04f;
	//static double time = 0;// Current time and elapsed time
	//					   //double turn = 0;    // Model turn counter
	//static double speed = 0.3; // Model rotation speed
	//static int wire = 0;       // Draw model in wireframe?
	//static float bgColor[] = { 0.1f, 0.2f, 0.4f };         // Background color
	//static unsigned char cubeColor[] = { 255, 0, 0, 128 }; // Model color (32bits RGBA)
	//													   // Add 'speed' to 'bar': it is a modifiable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [s] and [S].


	//TwAddVarRO(myBar, "NumTexUnits", TW_TYPE_UINT32, &NumberOfTexUnits, " label='# of texture units' ");
	//TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.

	//TwAddVarRW(myBar, "speed", TW_TYPE_DOUBLE, &speed,
	//	" label='Rot speed' min=0 max=2 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");

	//// Add 'wire' to 'bar': it is a modifiable variable of type TW_TYPE_BOOL32 (32 bits boolean). Its key shortcut is [w].
	//TwAddVarRW(myBar, "wire", TW_TYPE_BOOL32, &wire,
	//	" label='Wireframe mode' key=w help='Toggle wireframe display mode.' ");

	//// Add 'time' to 'bar': it is a read-only (RO) variable of type TW_TYPE_DOUBLE, with 1 precision digit
	//TwAddVarRO(myBar, "fps", TW_TYPE_DOUBLE, &m_fps, " label='fps' precision=1 help='FPS' ");

	//// Add 'bgColor' to 'bar': it is a modifiable variable of type TW_TYPE_COLOR3F (3 floats color)
	//TwAddVarRW(myBar, "bgColor", TW_TYPE_COLOR3F, &bgColor, " label='Background color' ");

	//// Add 'cubeColor' to 'bar': it is a modifiable variable of type TW_TYPE_COLOR32 (32 bits color) with alpha
	//TwAddVarRW(myBar, "cubeColor", TW_TYPE_COLOR32, &cubeColor,
	//	" label='Cube color' alpha help='Color and transparency of the cube.' ");
	//TwAddVarRW(myBar, "TessLevelInner", TW_TYPE_UINT32, &tessLevelInner, "label='TessLevelInner' min=1 max=32");
	//TwAddVarRW(myBar, "TessLevelOuter", TW_TYPE_UINT32, &tessLevelOuter, "label='TessLevelOuter' min=1 max=32");
	//TwAddVarRW(myBar, "Bumpiness", TW_TYPE_FLOAT, &bumpiness,
	//	" label='bumpiness' min=0 max=0.5 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");
}

void GUI::addBool(const std::string& label, bool& value)
{
	TwAddVarRW(m_twBar, label.c_str(), TW_TYPE_BOOLCPP, &value, "");
}

void GUI::addDouble(const std::string& label, double& value)
{
	TwAddVarRO(m_twBar, label.c_str(), TW_TYPE_DOUBLE, &value, "precision=1");
}

void GUI::run()
{
	TwDraw();
}

void GUI::clean()
{
	TwTerminate();
}
