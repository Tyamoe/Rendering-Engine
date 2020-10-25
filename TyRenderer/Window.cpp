#include "Window.h"

#include "WindowCB.h"
#include "Input.h"

#include "TyRenderer.h"

#include "Globals.h"
#include "GLUtils.h"
#include "ImGuiUtils.h"

TYumap<GLFWwindow*, Window*> WindowManager;

TYbool Window::Focus()
{
	glfwMakeContextCurrent(window);

	TyRenderer::width = layout.width;
	TyRenderer::height = layout.height;

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
		//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
	}

	TYint numMonitors = 0;
	GLFWmonitor** monitors = glfwGetMonitors(&numMonitors);

	if (numMonitors > 0)
	{
		for (TYint i = 0; i < numMonitors; i++)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

			Monitor monitor;
			monitor.width = mode->width;
			monitor.height = mode->height;
			monitor.refreshRate = mode->refreshRate;

			if (monitors[i] == glfwGetPrimaryMonitor())
			{
				PrimaryMonitor = i;
			}

			Monitors.push_back(monitor);
		}
	}
	else
	{
		Monitor monitor;
		monitor.width = 1920;
		monitor.height = 1080;
		monitor.refreshRate = 60;

		PrimaryMonitor = 0;

		Monitors.push_back(monitor);
	}

	if (settings.fullscreen)
	{
		layout.width = Monitors[PrimaryMonitor].width;
		layout.height = Monitors[PrimaryMonitor].height;

		layout.left = 0;
		layout.top = 0;
	}

	GLFWwindow* prevWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(NULL);

	window = glfwCreateWindow(layout.width, layout.height, name, NULL, MainWindow);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window: " << name << std::endl;
		glfwTerminate();

		WindowInitialized = false;
		return;
	}

	glfwMakeContextCurrent(window);

	if (!MainWindow)
	{
		TyRenderer::width = layout.width;
		TyRenderer::height = layout.height;
	}

	glfwWindowHint(GLFW_SAMPLES, settings.MSAA);
	if(settings.MSAA > 0) glEnable(GL_MULTISAMPLE);

	if (settings.vsync)
	{
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	}
	else
	{
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
	}

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
			MainWindow = window;
			Global::GlewInit = true;
		}
	}

	WindowManager[window] = this;

	glfwSetWindowPos(window, layout.left, layout.top);

	glfwSetFramebufferSizeCallback(window, ViewportCB);
	glfwSetWindowFocusCallback(window, FocusCB);

	glViewport(0, 0, layout.width, layout.height);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND_COLOR);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glDebugMessageCallback(ExErrorCB, NULL);
	//glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);

	glClearColor(0.3f, 0.3f, 0.5f, 1.0f);

	// Input Callbacks
	glfwSetKeyCallback(window, Input::KeyCB);
	glfwSetMouseButtonCallback(window, Input::MouseCB);
	glfwSetScrollCallback(window, Input::ScrollCB);
	glfwSetCursorPosCallback(window, Input::CursorCB);
	glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);

	if (settings.maximize)
	{
		glfwMaximizeWindow(window);
	}

	GLenum e;
	if ((e = glGetError()) != GL_NO_ERROR)
	{
		//TYlog << "5-00 " << glewGetErrorString(e) << TYlogbreak;
		printf("5-00 %s\n", glewGetErrorString(e));
	}

	glfwMakeContextCurrent(MainWindow);

	WindowInitialized = true;
}

Window::Window(TYcstring pName, Settings pSettings, Layout pLayout) : layout(pLayout), settings(pSettings)
{
	name = pName;

	cCreateWindow();
}

Window::~Window()
{
	if (WindowInitialized)
	{
		/*if (ImGUI Renderer Created)
		{
			ImGui_ImplOpenGL3_Shutdown();
		}*/

		glfwDestroyWindow(window);
		glfwTerminate();
	}
}