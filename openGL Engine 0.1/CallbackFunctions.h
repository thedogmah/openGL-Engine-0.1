#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "SSBO.h"
#include "Lights.h"

//Some callback functions depeend on other funtions preaturely declared here
int findCubeIndexByColour(const glm::vec3 color);
//openGL call back function declaration


void GLAPIENTRY  debugCallback(GLenum source, GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam);



//initialise debug openGL 4.3 + callback
void GLAPIENTRY  debugCallback(GLenum source, GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

bool boolRigidBody = false;
std::vector<World::cubeInstance> cubesSSBOVector;
GLuint colorFBO;
GLuint colorTexture;
GLuint depthrenderBuffer;
int rgbSelected[4];
SSBO* cubeSSBOptr = nullptr;
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);


	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {


		lMouseClicked = true;

		double xpos, ypos;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		glfwGetCursorPos(window, &xpos, &ypos);
		//std::cout << "\nglfw sees x: " << xpos << ", y: " << ypos << std::endl;
		int glX = static_cast<int>(xpos);
		int glY = height - static_cast<int>(ypos) - 1;

		GLubyte pixelColor[4];

		GLint currentFramebuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);
		GLenum error;
		if (currentFramebuffer == colorFBO) {
			// The colorFBO is currently bound as the framebuffer. You can safely call glReadPixels here

			glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;



			}
			//std::cout << "\nMouse co ordinates, X:" << glX << " Y:" << glY << std::endl;
			std::cout << "\n" << static_cast<int>(pixelColor[0]) << "," << static_cast<int>(pixelColor[1]) << ", " << static_cast<int>(pixelColor[2]) << ", " << static_cast<int>(pixelColor[3]);
			rgbSelected[0] = pixelColor[0];
			rgbSelected[1] = pixelColor[1];
			rgbSelected[2] = pixelColor[2];
			rgbSelected[3] = pixelColor[3];
			std::cout << "\nRGB seleted: " << rgbSelected[0] << ", " << rgbSelected[1] << ", " << rgbSelected[2];
			glm::vec3 colour;
			colour.r = rgbSelected[0];

			colour.g = rgbSelected[1];
			colour.b = rgbSelected[2];
			int deleteVector = -1;
			deleteVector = (findCubeIndexByColour(glm::vec3(pixelColor[0], pixelColor[1], pixelColor[2])));
			if (deleteVector != -1 && cubesSSBOVector.size() > 0) {

				//dynamicsWorldPtr->removeRigidBody(
				std::cout << "\nVector index to delete: " << deleteVector;
				std::cout << "\ncube ID: " << cubesSSBOVector[deleteVector].ID;
				dynamicsWorldPtr->removeRigidBody(cubesSSBOVector[deleteVector].rigidBody);
				cubesSSBOVector.erase(cubesSSBOVector.begin() + (deleteVector));
				cubeSSBOptr->updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());

				//for (int i = dynamicsWorldPtr->getNumCollisionObjects() - 1; i >= 0; i--) {
				//	btCollisionObject* obj = dynamicsWorldPtr->getCollisionObjectArray()[i];
				//	btRigidBody* rigidBody = btRigidBody::upcast(obj);

				//	if (rigidBody && rigidBody->getUserPointer() == reinterpret_cast<void*>(deleteVector)) {
				//		std::cout << "\nUser Pointer value of click cube: " << (int)rigidBody->getUserPointer();
				//		// Found the rigid body with the matching user data (ID)
				//		


				//		std::cout << "\nRigid Body Vector Size: "<<rigidBodyVectorPtr->size();
				//		
				//		std::cout << "\nRigid Body Vector Size: " << rigidBodyVectorPtr->size();

				//		std::cout << "\nNumber of collision objects" << dynamicsWorldPtr->getNumCollisionObjects();
				//		
				//		dynamicsWorldPtr->removeRigidBody(rigidBody);
				//		
				//		delete rigidBody->getMotionState();
				//		delete rigidBody->getCollisionShape();
				//		
				//		delete rigidBody;
				//		std::cout << "\nNumber of collision objects:" << dynamicsWorldPtr->getNumCollisionObjects();

				//		break; // Assuming each ID is unique, you can break out of the loop once you find a match.
				//	}
				//	rigidBodyVectorPtr->erase(rigidBodyVectorPtr->begin() + deleteVector);
				//}




			}
		}
		else {
			std::cout << "\nColor buffer not bound";
			glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
			while ((error = glGetError()) != GL_NO_ERROR) {
				//	std::cout << "OpenGL Error: " << error << std::endl;
				glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
				//	std::cout << "\nMouse co ordinates, X:" << glX << " Y:" << glY << std::endl;
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//std::cout << static_cast<int>(pixelColor[0]) << "," << static_cast<int>(pixelColor[1]) << ", " << static_cast<int>(pixelColor[2]) << ", " << static_cast<int>(pixelColor[3]);
		//rgbSelected[0] = pixelColor[0];
		//rgbSelected[1] = pixelColor[1];
		//rgbSelected[2] = pixelColor[2];
		//rgbSelected[3] = pixelColor[3];
		//glm::vec3 colour;
		//colour.r = rgbSelected[0];

		//colour.g = rgbSelected[1];
		//colour.b = rgbSelected[2];
		//int deleteVector = (findCubeIndexByColour(glm::vec3(pixelColor[0], pixelColor[1], pixelColor[2])));
		//if (deleteVector != -1) {
		//	std::cout << "vector idnex to delete: " << deleteVector;
		//	cubesSSBOVector.erase(cubesSSBOVector.begin() + deleteVector);
		//	cubeSSBOptr->updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
		//	deleteVector = 0;
		//	
		//	}



	}
	else {
		//Left Mouse Button is not clicked
		lMouseClicked = false;

	}


}