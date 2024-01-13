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
	glm::vec3& getPosition();
	void setWaterEye();
	void updatePosition();
	void updateOrientation();
	float getYaw();
	float getPitch();
	void flipPitch();
	GLFWwindow* m_window;
	glm::vec3 mPosition;
	glm::vec3 mFront; 
	glm::vec3 mUp;
	glm::mat4 mViewMatrix;
	glm::mat4 mReflectionMatrix;
	float mYaw;
	float mPitch;
	glm::vec3 mRight;
	float mMovementSpeed;
	float mMouseSensitivity; 
	float mZoom;
	Character* character = nullptr;
};

