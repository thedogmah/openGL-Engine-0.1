#include "Camera.h"
#include <cmath>
//#include <glm/glm.hpp>
Camera::Camera()	:
		mPosition(glm::vec3(0.0f, 1.0f, 3.0f)),
		mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
		mUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		mYaw(-90.0f),
		mPitch(0.0f),
		mMovementSpeed(0.35f),
		mMouseSensitivity(0.00013f),
		mZoom(45.0f)
	
{
	
}

void printMatrix(glm::mat4, std::string);
void Camera::Camera::update() {
	
	

	 {
		updateOrientation();
		updatePosition();
	}
}

glm::mat4 Camera::Camera::getViewMatrix() const {

	return glm::lookAt(mPosition, mPosition + mFront, +mUp);

}

void Camera::setViewMatrix()
{
	mViewMatrix = glm::lookAt(mPosition, mPosition + mFront, mUp);
}

glm::vec3& Camera::getPosition()
{
	//std::cout << "\nCamera Y is: " << mPosition.y;
	return mPosition;
}

void Camera::setWaterEye()
{
	mPosition.y = mPosition.y * 2;

}

void Camera::Camera::updateOrientation() {

	if (lMouseClicked)//debug bool for if camera is activated (not clicked) 

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

		//glfwSetCursorPos(m_window, window_width / 2, window_height / 2);


	}
	///glfwGetWindowSize(m_window, window_width, window_height);
	
}

float Camera::getYaw()
{
	return this->mYaw;
}

float Camera::getPitch()
{
	return this->mPitch;
}

void Camera::flipPitch()
{
	// Lower the camera position along the y-axis
	mPosition.y -= (2.0f * mPosition.y);

	// Flip the pitch
	mPitch = -mPitch;
	const float pitchClampMin = -89.0f;
	const float pitchClampMax = 89.0f;
	mPitch = glm::clamp(mPitch, pitchClampMin, pitchClampMax);

	// Update mFront and mUp based on the new pitch
	mFront = glm::normalize(glm::vec3(
		cos(glm::radians(mYaw)) * cos(glm::radians(mPitch)),
		sin(glm::radians(mPitch)),
		sin(glm::radians(mYaw)) * cos(glm::radians(mPitch))
	));
//	mUp = glm::normalize(glm::cross(glm::vec3(cos(glm::radians(mYaw)), 0.0f, sin(glm::radians(mYaw))), mFront));

	// Update the mViewMatrix using glm::lookAt
	mViewMatrix = glm::lookAt(mPosition, mPosition + mFront, mUp);

	// Print the updated mViewMatrix
	printMatrix(mViewMatrix, "Reflection View Matrix");

}





void Camera::Camera::updatePosition() {

	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_REPEAT)

	{
		character->characterActive = false;

	}
	//Switch Mouse Clicked on spacekey
	if (true) {
		if (glfwGetKey(m_window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_CAPS_LOCK) == GLFW_REPEAT)
		{
			this->mMouseSensitivity = 0.0;

			this->mMovementSpeed = 0.0;
			if (this->cameraActive)
				this->cameraActive = false;
			if (this->cameraActive)
				this->cameraActive = true;
			if (lMouseClicked == true) { lMouseClicked = false; }
			if (lMouseClicked == false) { lMouseClicked = true; }
		}


		if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_REPEAT)
		{
			this->mMovementSpeed = 0.35f,
				this->mMouseSensitivity = 0.00033f;
		}


		if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_REPEAT)

		{
			mPosition += glm::vec3(0.0, 0.1, 0.f);

		}

		if (glfwGetKey(m_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_REPEAT)

		{
			mPosition -= glm::vec3(0.0, 0.1, 0.f);

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
			// Move the camera left (relative to its current orientation)
			mPosition -= glm::normalize(glm::cross(mFront, glm::vec3(0.0f, 1.0f, 0.0f))) * mMovementSpeed;
		}

		if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_D) == GLFW_REPEAT)
		{
			// Move the camera right (relative to its current orientation)
			mPosition += glm::normalize(glm::cross(mFront, glm::vec3(0.0f, 1.0f, 0.0f))) * mMovementSpeed;
		}

		if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_W) == GLFW_REPEAT)
		{
			// Move the camera forward (in the direction it's currently facing)
			mPosition += mFront * mMovementSpeed;
		}

		if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_S) == GLFW_REPEAT)
		{
			// Move the camera backward (opposite direction of where it's facing)
			mPosition -= mFront * mMovementSpeed;
		}
	}
}