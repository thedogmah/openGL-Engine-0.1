#pragma once
//#include "globals.h"
#include <btBulletDynamicsCommon.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class Character
{
public:
	Character(btDynamicsWorld* world, float initialX, float initialY, float initialZ, GLFWwindow* window);

	void handleInput();
	void updatePhysics();
	void update();
	void debug();
	btRigidBody* getRigidBody();
private:
	btRigidBody* rigidBody = nullptr;
	float moveSpeed =0.1;
	float jumpForce =0.1;

	void initCharacter(btDynamicsWorld* world, float initialX, float initialY, float initialZ);
	void applyMovement();
	bool characterActive = true;
	float frictionValue = 1.0;
	float forceFactor = 1.0;
	GLFWwindow* window = nullptr;
	btVector3 force= btVector3(0., 0., 0.);
};

