#include "GLError.hpp"
#include <iostream>
#include <string>

//#if defined(WIN32) || defined(_WIN64)
//#  include <GL/glew.h>
//#elif __APPLE__
//#include <OpenGLES/ES3/gl.h>
//#include <OpenGLES/ES3/glext.h>
//#else
//#  include <GL3/gl3.h>
//#endif

#include "RFGL.hpp"
#include "Log.hpp"

using namespace std;

void _checkOpenGLError(const char *file, int line) {
	GLenum err(glGetError());

	while (err != GL_NO_ERROR) {
		string error;

		switch (err) {
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

        //SDL_Log("error");
		//cerr << "GL_" << error.c_str() << " - " << file << ":" << line << endl;
		warning("GL_%s - %s:%d", error.c_str(), file, line);
		err = glGetError();
	}
}