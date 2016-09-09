#ifndef GLERROR_H
#define GLERROR_H

void _checkOpenGLError(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
//#ifdef _DEBUG
//#define check_gl_error() _check_gl_error(__FILE__,__LINE__)
//#else
//#define check_gl_error()
//#endif
#define glCheckError() _checkOpenGLError(__FILE__,__LINE__)

#endif // GLERROR_H