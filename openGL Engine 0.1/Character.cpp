#include "Character.h"

Character::Character(btDynamicsWorld* world, float initialX, float initialY, float initialZ)
{
	btCollisionShape* characterShape = new btCapsuleShape(0.5, 1);
	
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(initialX, initialY, initialZ));

	//create mass for character
	btScalar mass = 1.0;
	btVector3 localInertia(0, 0, 0);
	characterShape->calculateLocalInertia(mass, localInertia);
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo CI(mass, motionState, characterShape, localInertia);
	rigidBody = new btRigidBody(CI);
	world->addRigidBody(rigidBody);
}
