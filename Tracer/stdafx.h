#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vendor\imgui\imgui.h"
#include "vendor\imgui\imgui_impl_glfw.h"
#include "vendor\imgui\imgui_impl_opengl3.h"
#include "vendor\imgui\imgui_internal.h"

#include <string>
#include <vector>
#include <map>
#include <unordered_map> 
#include <set>
#include <unordered_set>

#include <iostream>
#include <cstddef>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std::chrono;

#include "Assert.h"


/* Set to 0 when building in release mode, set to 1 when building in debug mode */
#define DE_BUG 0

#define UNREFERENCED_PARAMETER(P) (P)

#define arraySize(A) ((int)(sizeof(A)/sizeof(*A)))
