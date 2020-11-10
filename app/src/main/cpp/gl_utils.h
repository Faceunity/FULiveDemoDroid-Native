#ifndef FULIVENATIVEDEMO_GL_UTILS_H
#define FULIVENATIVEDEMO_GL_UTILS_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "android_util.h"

void checkGlError(char *op);

GLuint loadShader(GLenum type, const char *source);

void createProgram();

void drawFrame(GLuint textureId, float *texMatrix);

void releaseProgram();

#endif //FULIVENATIVEDEMO_GL_UTILS_H
