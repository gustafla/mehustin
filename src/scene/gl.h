#ifdef GLES
#include <GLES2/gl2.h>
#else // !defined(GLES)
#ifdef __MINGW64__
#include <GL/glew.h>
#else // !defined(__MINGW64__)
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif // !defined(GLES) && !defined(__MINGW64__)
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif
