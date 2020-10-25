#pragma once

#include "Types.h"
#include "GLUtils.h"

TYvoid FocusCB(GLFWwindow*, TYint);
TYvoid ErrorCB(TYint error, const char* description);
TYvoid ViewportCB(GLFWwindow* window, TYint width, TYint height);
TYvoid GLAPIENTRY ExErrorCB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
