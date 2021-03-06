#include "GLError.hpp"
#include <iostream>
#include <string>

#include "RFGL.hpp"
#include "Debug.hpp"

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
        Debug::LogWarning("GL_%s - %s:%d", error.c_str(), file, line);
        err = glGetError();
    }
}