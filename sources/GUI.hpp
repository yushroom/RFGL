#ifndef GUI_hpp
#define GUI_hpp

#include <string>
#include "RFGL.hpp"
#include "IRunable.hpp"
#include <AntTweakBar.h>

//struct TwBar;

class GUI
{
public:
	GUI() = delete;

	static void init();

	static void addBool(const std::string& label,  bool& value);

	static void addDouble(const std::string& label, double& value);

	static void run();

	static void clean();

private:
	static TwBar *m_twBar;
};

#endif // GUI_hpp