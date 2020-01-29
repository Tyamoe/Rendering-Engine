#pragma once

#include "stdafx.h"

typedef class Camera
{
public:
	Camera();
	~Camera();

	_inline bool Dirty() { return dirty; }
	_inline void setDirty(bool d) { dirty = d; }

	_inline void setPosition(glm::vec3& pos)
	{
		Position = pos;
		dirty = true;
	}

	_inline float getZoom() { return zoom; }

	_inline void setZoom(float z)
	{
		zoom = z;
		dirty = true;
	}

	_inline glm::mat4* View() { return &view; }

	_inline glm::mat4* ProjView() { return &PV; }

	_inline glm::vec3* getPosition() { return &Position; }

	_inline void setPosition(glm::vec3 *pos) { Position = *pos; }

	_inline float& getSpeed() { return speed; }

	_inline void setSpeed(float s) { speed = s; }

	_inline void setSmoothness(float sm) { smoothness = sm; }

	_inline float& getSmoothness() { return smoothness; }

	_inline void setTarget(glm::vec2 t) { Target = t; }

	_inline glm::vec2& getTarget() { return Target; }

	void Update(float dt);

	void getWorldPosition(float* x, float* y);


	void ChangeCameraZoom(float scale);

	//void zoomToPos(float amount, glm::vec2 pos);

	glm::vec2 TargetWorldPosition;
	// Current View matrix.
	glm::mat4 view = glm::mat4(1.0f);
	// Camera Transform -> used to make View matrix.
	glm::vec3 Position = glm::vec3(-4.30f, 5.5f, 6.4f);
	glm::vec3 Front = glm::vec3(0.5f, -0.5f, -0.6f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	float yaw_ = -83.4f, pitch_ = -35;
private:
	std::string name;

	bool dirty = true;
	//              (tm)
	glm::vec2 Target;

	// Camera zoom.
	float zoom = 25.f;

	float speed = 80;
	//the number is multiplied by 200 to give more granularity to the user
	float smoothness = 100 * .05f;
	float smoothDiv = 200;

	// Current Projection matrix.
	glm::mat4 proj;
	// Projection/View matrix concat
	glm::mat4 PV;

}Camera, *CameraPtr;