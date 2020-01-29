#pragma once

#include "stdafx.h"
#include <glm/gtc/type_ptr.hpp>

typedef enum MouseButton
{
	MouseLeft,
	MouseRight,
	MouseMiddle,
}MouseButton;

struct Keyboard
{
	bool keyDown[350];
	bool keyReleased[350];
	bool keyPressed[350];

	bool keyCtrl;
	bool keyShift;
	bool keyAlt;
};

struct Mouse
{
	bool mouseDown[5];
	bool mouseReleased[5];
	bool mousePressed[5];

	float wheelHorizontal = 0.0f;
	float wheelVertical = 0.0f;

	float wheelVertDiff = 0.0f;

	glm::vec2 screenPos;
	glm::vec2 worldPos;
	glm::vec2 prevScreenPos; // penis
	glm::vec2 screenOffset;
};

namespace Input
{
	extern Keyboard keyboard;
	extern Mouse mouse;

	void Update(float dt);

	bool isKeyDown(int key);
	bool isKeyReleased(int key);
	bool isKeyPressed(int key);

	bool isMousePressed(MouseButton button);
	bool isMouseDown(MouseButton button);
	bool isMouseReleased(MouseButton button);

	glm::vec2 * getMouseWorldPosition();

	Mouse getMouse();
	Keyboard getKeyboard();
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mousePosition(GLFWwindow *pwin, double xpos, double ypos);

void UpdateMouseWorldPosition();

void mouseButton(GLFWwindow* window, int button, int action, int mods);

void mouseScroll(GLFWwindow* pwin, double xoffset, double yoffset);