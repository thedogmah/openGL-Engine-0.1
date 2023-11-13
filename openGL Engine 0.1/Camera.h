#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "globals.h"
#include "Character.h"
class Character;
class Camera
{
public:
	Camera();
	bool cameraActive = true;
	bool locked = false;
	void update();
	glm::mat4 getViewMatrix() const;
	void setViewMatrix();


	void updatePosition();
	void updateOrientation();
	float getYaw();
	float getPitch();
	GLFWwindow* m_window;
	glm::vec3 mPosition;
	glm::vec3 mFront; 
	glm::vec3 mUp;
	glm::mat4 mViewMatrix;
	float mYaw;
	float mPitch;

	float mMovementSpeed;
	float mMouseSensitivity; 
	float mZoom;
	Character* character = nullptr;
};

