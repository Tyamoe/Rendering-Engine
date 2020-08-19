#pragma once
#define WIN32_LEAN_AND_MEAN
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vendor\imgui\imgui.h"
#include "vendor\imgui\imgui_impl_glfw.h"
#include "vendor\imgui\imgui_impl_opengl3.h"
#include "vendor\imgui\imgui_internal.h"

#include <iostream>
#include <cstddef>

#define DE_BUG 0

#define UNREFERENCED_PARAMETER(P) (P)

#define arraySize(A) ((int)(sizeof(A)/sizeof(*A)))
