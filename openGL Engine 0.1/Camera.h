#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "globals.h"
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

};

