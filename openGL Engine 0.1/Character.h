#pragma once
//#include "globals.h"
#include <btBulletDynamicsCommon.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "globals.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Character
{
public:
	Character(btDynamicsWorld* world, float initialX, float initialY, float initialZ, GLFWwindow* window);
	
	void handleInput();
	void updatePhysics();
	void update();
	void debug();
	btRigidBody* getRigidBody();
	bool characterActive = false;
private:
	btRigidBody* rigidBody = nullptr;
	float moveSpeed =0.1;
	float jumpForce =0.1;

	void initCharacter(btDynamicsWorld* world, float initialX, float initialY, float initialZ);
	void applyMovement();
	
	float frictionValue = 1.0;
	float forceFactor = 1.0;
	GLFWwindow* window = nullptr;
	glm::vec3 force= glm::vec3(0., 0., 0.);

	//Character Camera variables
	glm::vec3 cameraDirection;
	float cameraYaw;     // Get the camera's yaw angle in radians
	float cameraPitch; // Get the camera's pitch angle in radians
	float linearDamping = 0.2f; // Adjust as needed
	float angularDamping = 0.2f; // Adjust as needed
};

