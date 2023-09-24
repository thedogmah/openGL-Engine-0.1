#pragma once
#include "globals.h"
#include <btBulletDynamicsCommon.h>

class Character
{
public:
	Character(btDynamicsWorld* world, float initialX, float initialY, float initialZ);

	void handleInput();
	void updatePhysics();


private:
	btRigidBody* rigidBody = nullptr;
	float moveSpeed;
	float jumpForce;

	void initCharacter(btDynamicsWorld* world, float initialX, float initialY, float initialZ);
	void applyMovement(float forward, float right);

};

