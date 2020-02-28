#include "stdafx.h"

#include "Input.h"
#include "Window.h"

TYumap<GLFWwindow*, Input*> InputManagers;
#define INPUT WindowManager[window]->GetInput()

Input::Input(GLFWwindow* pWindow)
{
	window = pWindow;
	glfwSetKeyCallback(window, KeyCB);
	glfwSetMouseButtonCallback(window, MouseCB);
	glfwSetScrollCallback(window, ScrollCB);
	glfwSetCursorPosCallback(window, CursorCB);
	glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);

	InputManagers[window] = this;
}

TYvoid Input::Update(TYfloat dt)
{
	for (TYint i = 0; i < 350; i++)
	{
		keyboard.keyPressed[i] = false;
		keyboard.keyReleased[i] = false;
	}

	for (TYint i = 0; i < 5; i++)
	{
		mouse.mousePressed[i] = false;
		mouse.mouseReleased[i] = false;
	}

	mouse.screenOffset = TYvec2(0);
	mouse.wheelVertDiff = 0;
}

TYvec2* Input::getMouseWorldPosition()
{
	return &mouse.worldPos;
}

TYvoid Input::UpdateMouseWorldPosition()
{
	TYfloat x, y;
	x = mouse.screenPos.x;
	y = mouse.screenPos.y;

	mouse.worldPos.x = x;
	mouse.worldPos.y = y;
}

TYbool Input::isMousePressed(MouseButton button)
{
	return mouse.mousePressed[button];
}
TYbool Input::isMouseDown(MouseButton button)
{
	return mouse.mouseDown[button];
}
TYbool Input::isMouseReleased(MouseButton button)
{
	return mouse.mouseReleased[button];
}

TYbool Input::isKeyDown(TYint key)
{
	ImGuiIO& io = ImGui::GetIO();
	return io.KeysDown[key];
}

TYbool Input::isKeyReleased(TYint key)
{
	return keyboard.keyReleased[key];
}

TYbool Input::isKeyPressed(TYint key)
{
	return keyboard.keyPressed[key];
}

TYvoid Input::KeyCB(GLFWwindow* window, TYint key, TYint scancode, TYint action, TYint mods)
{
	if (key < 0) return;

	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(TYvoid)mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		
	}
	
	if (action == GLFW_PRESS)
	{
		INPUT->keyboard.keyPressed[key] = true;
		INPUT->keyboard.keyDown[key] = true;
		INPUT->keyboard.keyReleased[key] = false;
	}

	if (action == GLFW_RELEASE)
	{
		INPUT->keyboard.keyPressed[key] = false;
		INPUT->keyboard.keyDown[key] = false;
		INPUT->keyboard.keyReleased[key] = true;
	}

	(TYvoid)mods; // Modifiers are not reliable across systems
	INPUT->keyboard.keyCtrl = INPUT->keyboard.keyDown[GLFW_KEY_LEFT_CONTROL] || INPUT->keyboard.keyDown[GLFW_KEY_RIGHT_CONTROL];
	INPUT->keyboard.keyShift = INPUT->keyboard.keyDown[GLFW_KEY_LEFT_SHIFT] || INPUT->keyboard.keyDown[GLFW_KEY_RIGHT_SHIFT];
	INPUT->keyboard.keyAlt = INPUT->keyboard.keyDown[GLFW_KEY_LEFT_ALT] || INPUT->keyboard.keyDown[GLFW_KEY_RIGHT_ALT];

}

TYvoid Input::CursorCB(GLFWwindow* window, TYdouble xpos, TYdouble ypos)
{
	TYfloat x = (TYfloat)xpos, y = (TYfloat)ypos;
	INPUT->mouse.screenPos.x = x;
	INPUT->mouse.screenPos.y = y;

	INPUT->mouse.worldPos.x = x;
	INPUT->mouse.worldPos.y = y;

	INPUT->mouse.screenOffset.x = x - INPUT->mouse.prevScreenPos.x;
	INPUT->mouse.screenOffset.y = INPUT->mouse.prevScreenPos.y - y;
	INPUT->mouse.prevScreenPos = TYvec2(x, y);
	INPUT->mouse.screenOffset *= .05f;
}

TYvoid Input::MouseCB(GLFWwindow* window, TYint button, TYint action, TYint mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	if (action == GLFW_PRESS && button >= 0)
	{
		INPUT->mouse.mouseDown[button] = true;
		INPUT->mouse.mousePressed[button] = true;
	}
	if (action == GLFW_RELEASE && button >= 0)
	{
		INPUT->mouse.mousePressed[button] = false;
		INPUT->mouse.mouseDown[button] = false;
		INPUT->mouse.mouseReleased[button] = true;
	}
	std::cout << INPUT->mouse.screenPos.x << ", " << INPUT->mouse.screenPos.y << std::endl;
}

TYvoid Input::ScrollCB(GLFWwindow* window, TYdouble xoffset, TYdouble yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (TYfloat)xoffset;
	io.MouseWheel += (TYfloat)yoffset;

	INPUT->mouse.wheelHorizontal += (TYfloat)xoffset;
	INPUT->mouse.wheelVertDiff = (TYfloat)yoffset;
	INPUT->mouse.wheelVertical += (TYfloat)yoffset;
}