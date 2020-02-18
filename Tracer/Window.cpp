#include "stdafx.h"

#include "WindowCB.h"
#include "Input.h"

#include "Window.h"
#include "RendererRayTrace.h"

void Window::AttachEngine(EnginePtr pEngine)
{
	engine = pEngine;
	engine->SetWindow(this, window);

	engine->Init();
	engine->Start();
}

void Window::AttachInput(InputPtr pInput)
{
	input = pInput;
}

void Window::CreateWindow(Layout pLayout)
{
	glfwInit();
	glfwSetErrorCallback(ErrorCB);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);

	// MSAA
	glfwWindowHint(GLFW_SAMPLES, MSAA);
	glEnable(GL_MULTISAMPLE);

	if (vsync)
	{
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	}

	//const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	GLint width = pLayout.width == 0 ? pLayout.width : pLayout.width;
	GLint height = pLayout.height == 0 ? pLayout.height : pLayout.height;

	layout.width = width;
	layout.height = height;

	window = glfwCreateWindow(width, height, "Ray-Tracer", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		//exit(-1);

		WindowInitialized = false;
		return;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, ViewportCB);

	glfwSetWindowPos(window, layout.left, layout.top);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		//exit(-1);

		WindowInitialized = false;
		return;
	}

	InputPtr input = new Input(window);
	AttachInput(input);
	//glfwSetKeyCallback(window, KeyCallback);
	//glfwSetMouseButtonCallback(window, mouseButton);
	//glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
	//glfwSetScrollCallback(window, mouseScroll);
	//glfwSetCursorPosCallback(window, mousePosition);

	glViewport(0, 0, width, height);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND_COLOR);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glClearColor(0.3f, 0.3f, 0.5f, 1.0f);

	glDebugMessageCallback(ExErrorCB, NULL);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	WindowInitialized = true;
}

Window::Window(Settings pSettings, Layout pLayout) : layout(pLayout)
{
	vsync = pSettings.vsync;
	MSAA = pSettings.MSAA;

	CreateWindow(pLayout);
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