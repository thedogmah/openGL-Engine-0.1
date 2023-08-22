#include "Camera.h"
#include "globals.h"
//#include <glm/glm.hpp>
Camera::Camera(GLFWwindow* window)		: m_window(window),
		mPosition(glm::vec3(0.0f, 0.5f, 3.0f)),
		mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
		mUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		mYaw(-90.0f),
		mPitch(0.0f),
		mMovementSpeed(0.2f),
		mMouseSensitivity(0.00013f),
		mZoom(45.0f)
{


}

void Camera::Camera::update() {
	updateOrientation();
	updatePosition();
	
}

glm::mat4 Camera::Camera::getViewMatrix() const {

	return glm::lookAt(mPosition, mPosition + mFront, + mUp);

}

void Camera::setViewMatrix()
{
	mViewMatrix = glm::lookAt(mPosition, mPosition + mFront, mUp);
}

void Camera::Camera::updateOrientation() {

	if (lMouseClicked)

	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		double xPos;
		double yPos;
	
		glfwGetCursorPos(m_window, &xPos, &yPos);

		static double lastX = xPos;
		static double lastY = yPos;

		double xOffset = xPos - lastX;
		double yOffset = lastY - yPos; // revese since y -coordinates range from bottom to top
	
		// Mouse sensitivity adjustment
		xOffset *= mMouseSensitivity;
		yOffset *= mMouseSensitivity;

		// Update yaw and pitch angles based on mouse movement
		mYaw += xOffset;
		mPitch += yOffset;
		
		// Clamp pitch to avoid flipping the camera upside down
		if (mPitch > 89.0f)
			mPitch = 89.0f;
		if (mPitch < -89.0f)
			mPitch = -89.0f;
	
		glm::vec3 front;
		front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		front.y = sin(glm::radians(mPitch));
		front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		mFront = glm::normalize(front);
	
	
	}


}



void Camera::Camera::updatePosition() {


	//Switch Mouse Clicked on spacekey

	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_REPEAT)

	{
		mPosition += glm::vec3(0.0, 0.01, 0.f);
		
	}

	if (glfwGetKey(m_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_REPEAT)

	{
		mPosition -= glm::vec3(0.0, 0.01, 0.f);

	}


	if (lMouseClicked == true)
	{
			
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	}

	else {
	
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_A) == GLFW_REPEAT)

	{
		mPosition.x -= 0.03;
	
	}

	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_D) == GLFW_REPEAT)

	{
		mPosition.x += 0.03;

	}

	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_W) == GLFW_REPEAT)

	{
		mPosition.z -= 0.02;

	}

	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_S) == GLFW_REPEAT)

	{
		mPosition.z += 0.02;

	}

}