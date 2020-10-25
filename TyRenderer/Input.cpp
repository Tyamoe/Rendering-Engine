#include "Input.h"

#include "Window.h"

#include "ImGuiUtils.h"
#include "GLUtils.h"

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

TYvec2 Input::GetMouseWorldPosition()
{
	return mouse.worldPos;
}

TYvoid Input::UpdateMouseWorldPosition()
{
	TYfloat x, y;
	x = mouse.screenPos.x;
	y = mouse.screenPos.y;

	mouse.worldPos.x = x;
	mouse.worldPos.y = y;
}

GLFWwindow* Input::GetHighlightWindow()
{
	return highlightedWindow;
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
	return keyboard.keyDown[key];
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
	if (action == GLFW_PRESS) io.KeysDown[key] = true;
	if (action == GLFW_RELEASE) io.KeysDown[key] = false;

	(TYvoid)mods;
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	
	if (action == GLFW_PRESS)
	{
		Input::keyboard.keyPressed[key] = true;
		Input::keyboard.keyDown[key] = true;
		Input::keyboard.keyReleased[key] = false;
	}

	if (action == GLFW_RELEASE)
	{
		Input::keyboard.keyPressed[key] = false;
		Input::keyboard.keyDown[key] = false;
		Input::keyboard.keyReleased[key] = true;
	}

	Input::keyboard.keyCtrl = Input::keyboard.keyDown[GLFW_KEY_LEFT_CONTROL] || Input::keyboard.keyDown[GLFW_KEY_RIGHT_CONTROL];
	Input::keyboard.keyShift = Input::keyboard.keyDown[GLFW_KEY_LEFT_SHIFT] || Input::keyboard.keyDown[GLFW_KEY_RIGHT_SHIFT];
	Input::keyboard.keyAlt = Input::keyboard.keyDown[GLFW_KEY_LEFT_ALT] || Input::keyboard.keyDown[GLFW_KEY_RIGHT_ALT];

}

TYvoid Input::CursorCB(GLFWwindow* window, TYdouble xpos, TYdouble ypos)
{
	//TYlog << InputManagers[window]->name << " | " << xpos << ", " << ypos << TYlogbreak;

	highlightedWindow = window;

	TYfloat x = (TYfloat)xpos, y = (TYfloat)ypos;
	Input::mouse.screenPos.x = x;
	Input::mouse.screenPos.y = y;

	Input::mouse.worldPos.x = x;
	Input::mouse.worldPos.y = y;

	Input::mouse.screenOffset.x = x - Input::mouse.prevScreenPos.x;
	Input::mouse.screenOffset.y = Input::mouse.prevScreenPos.y - y;
	Input::mouse.prevScreenPos = TYvec2(x, y);
	Input::mouse.screenOffset *= 0.05f;
}

TYvoid Input::MouseCB(GLFWwindow* window, TYint button, TYint action, TYint mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	if (action == GLFW_PRESS && button >= 0)
	{
		Input::mouse.mouseDown[button] = true;
		Input::mouse.mousePressed[button] = true;
	}
	if (action == GLFW_RELEASE && button >= 0)
	{
		Input::mouse.mousePressed[button] = false;
		Input::mouse.mouseDown[button] = false;
		Input::mouse.mouseReleased[button] = true;
	}
	//TYlog << Input::mouse.screenPos.x << ", " << Input::mouse.screenPos.y << TYlogbreak;
}

TYvoid Input::ScrollCB(GLFWwindow* window, TYdouble xoffset, TYdouble yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (TYfloat)xoffset;
	io.MouseWheel += (TYfloat)yoffset;

	Input::mouse.wheelHorizontal += (TYfloat)xoffset;
	Input::mouse.wheelVertDiff = (TYfloat)yoffset;
	Input::mouse.wheelVertical += (TYfloat)yoffset;
}