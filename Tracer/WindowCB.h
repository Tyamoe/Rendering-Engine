#pragma once

#include "stdafx.h"

void ErrorCB(int error, char const* description);
void ViewportCB(GLFWwindow* window, int width, int height);
void GLAPIENTRY ExErrorCB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
