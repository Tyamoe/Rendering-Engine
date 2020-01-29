#include "stdafx.h"

#include "Render.h"
#include "Input.h"

Keyboard Input::keyboard;
Mouse Input::mouse;

void Input::Update(float dt)
{
	for (int i = 0; i < 350; i++)
	{
		keyboard.keyPressed[i] = false;
		keyboard.keyReleased[i] = false;
	}

	for (int i = 0; i < 5; i++)
	{
		mouse.mousePressed[i] = false;
		mouse.mouseReleased[i] = false;
	}

	mouse.screenOffset = glm::vec2(0);
	mouse.wheelVertDiff = 0;
}

glm::vec2* Input::getMouseWorldPosition()
{
	return &mouse.worldPos;
}

bool Input::isMousePressed(MouseButton button)
{
	return mouse.mousePressed[button];
}
bool Input::isMouseDown(MouseButton button)
{
	return mouse.mouseDown[button];
}
bool Input::isMouseReleased(MouseButton button)
{
	return mouse.mouseReleased[button];
}

bool Input::isKeyDown(int key)
{
	ImGuiIO& io = ImGui::GetIO();
	return io.KeysDown[key];
}

bool Input::isKeyReleased(int key)
{
	return keyboard.keyReleased[key];
}

bool Input::isKeyPressed(int key)
{
	return keyboard.keyPressed[key];
}

Mouse Input::getMouse()
{
	return mouse;
}

Keyboard Input::getKeyboard()
{
	return keyboard;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key < 0) return;

	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(void)mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		
	}

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

	(void)mods; // Modifiers are not reliable across systems
	Input::keyboard.keyCtrl = Input::keyboard.keyDown[GLFW_KEY_LEFT_CONTROL] || Input::keyboard.keyDown[GLFW_KEY_RIGHT_CONTROL];
	Input::keyboard.keyShift = Input::keyboard.keyDown[GLFW_KEY_LEFT_SHIFT] || Input::keyboard.keyDown[GLFW_KEY_RIGHT_SHIFT];
	Input::keyboard.keyAlt = Input::keyboard.keyDown[GLFW_KEY_LEFT_ALT] || Input::keyboard.keyDown[GLFW_KEY_RIGHT_ALT];

}

void mousePosition(GLFWwindow *pwin, double xpos, double ypos)
{
	float x = (float)xpos, y = (float)ypos;
	Input::mouse.screenPos.x = x;
	Input::mouse.screenPos.y = y;

	Input::mouse.worldPos.x = x;
	Input::mouse.worldPos.y = y;

	Input::mouse.screenOffset.x = x - Input::mouse.prevScreenPos.x;
	Input::mouse.screenOffset.y = Input::mouse.prevScreenPos.y - y;
	Input::mouse.prevScreenPos = glm::vec2(x, y);
	Input::mouse.screenOffset *= .05f;
}

void UpdateMouseWorldPosition()
{
	float x, y;
	x = Input::mouse.screenPos.x;
	y = Input::mouse.screenPos.y;

	Input::mouse.worldPos.x = x;
	Input::mouse.worldPos.y = y;
}

void mouseButton(GLFWwindow* window, int button, int action, int mods)
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
}

void mouseScroll(GLFWwindow*, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (float)xoffset;
	io.MouseWheel += (float)yoffset;

	Input::mouse.wheelHorizontal += (float)xoffset;
	Input::mouse.wheelVertDiff = (float)yoffset;
	Input::mouse.wheelVertical += (float)yoffset;
}