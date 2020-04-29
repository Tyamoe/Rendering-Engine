#include "stdafx.h"

#include "Globals.h"
#include "WindowCB.h"
#include "Input.h"

#include "Window.h"
#include "RendererRayTrace.h"

TYumap<GLFWwindow*, WindowPtr> WindowManager;

TYvoid Window::AttachEngine(EnginePtr pEngine, bool pStart)
{
	engine = pEngine;
	engine->SetWindow(this, window);

	engine->Init();

	if (pStart)
	{
		engine->Start();
	}
}

TYvoid Window::AttachInput(InputPtr pInput)
{
	input = pInput;
}

TYbool Window::Focus()
{
	glfwMakeContextCurrent(window);

	// MSAA
	glfwWindowHint(GLFW_SAMPLES, MSAA);
	glEnable(GL_MULTISAMPLE);

	if (vsync)
	{
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	}

	return true;
}

TYvoid Window::cCreateWindow()
{
	if (!Global::GLFWInit)
	{
		Global::GLFWInit = true;

		glfwInit();
		glfwSetErrorCallback(ErrorCB);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
	}

	int numberOfMonitors = 0;
	GLFWmonitor** monitors = glfwGetMonitors(&numberOfMonitors);
	//const GLFWvidmode* desktopMode = glfwGetVideoMode(monitors[0]);

	//const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	if (fullscreen)
	{
		layout.width = 1920;// desktopMode->width;
		layout.height = 1080;// desktopMode->height;

		layout.left = 0;
		layout.top = 0;
	}

	GLint width = layout.width == 0 ? /*desktopMode->width*/1920 : layout.width;
	GLint height = layout.height == 0 ? /*desktopMode->height*/1080 - 63 : layout.height;

	layout.width = width;
	layout.height = height;

	window = glfwCreateWindow(width, height, name, NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window: " << name << std::endl;
		glfwTerminate();

		WindowInitialized = false;
		return;
	}

	Focus();

	if (!Global::GlewInit)
	{
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
			glfwTerminate();

			WindowInitialized = false;
			return;
		}
		else
		{
			Global::GlewInit = true;
		}
	}

	WindowManager[window] = this;

	glfwSetWindowPos(window, layout.left, layout.top);

	InputPtr input = new Input(window);
	AttachInput(input);

	glfwSetFramebufferSizeCallback(window, ViewportCB);

	glViewport(0, 0, width, height);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND_COLOR);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glDebugMessageCallback(ExErrorCB, NULL);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	glClearColor(0.3f, 0.3f, 0.5f, 1.0f);

	WindowInitialized = true;

	if (maximize)
	{
		glfwMaximizeWindow(window);
	}
}

Window::Window(TYcstring pName, Settings pSettings, Layout pLayout) : layout(pLayout)
{
	maximize = pSettings.maximize;
	vsync = pSettings.vsync;
	MSAA = pSettings.MSAA;
	fullscreen = pSettings.fullscreen;

	name = pName;

	cCreateWindow();
}

Window::~Window()
{
	if (WindowInitialized)
	{
		delete input;
		delete engine;

		/*if (ImGUI Renderer Created)
		{
			ImGui_ImplOpenGL3_Shutdown();
		}*/

		glfwDestroyWindow(window);
		glfwTerminate();
	}
}