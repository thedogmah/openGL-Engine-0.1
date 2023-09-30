#include "Character.h"

Character::Character(btDynamicsWorld* world, float initialX, float initialY, float initialZ, GLFWwindow* window)
{
	btCollisionShape* characterShape = new btCapsuleShape(1, 1.0);
	this->window = window;
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(initialX, initialY, initialZ));

	//create mass for character
	btScalar mass = 0.1;
	btVector3 localInertia(0, 0, 0);
	characterShape->calculateLocalInertia(mass, localInertia);
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo CI(mass, motionState, characterShape, localInertia);
	rigidBody = new btRigidBody(CI);
	rigidBody->forceActivationState(ACTIVE_TAG);
	rigidBody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
	world->addRigidBody(rigidBody);
}

void Character::handleInput()
{
	if (characterActive) {
		rigidBody->forceActivationState(ACTIVE_TAG);
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_T) == GLFW_REPEAT) {
			//moveForward();
			force += btVector3(0.0, 0.0, -1.0);
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			//	moveBackward();
			force += btVector3(0.0, 0.0, 1.0);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			// Increment the leftward force (to the left of the character's forward direction)
			force += btVector3(-1.0, 0.0, 0.0);
		}

		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			// Increment the rightward force (to the right of the character's forward direction)
			force += btVector3(1.0, 0.0, 0.0);
		}
	}


}

void Character::update()
{
	handleInput();
	applyMovement();
	
	debug();
}

void Character::debug()
{
	if (ImGui::GetCurrentContext()) {
	
		ImGui::Begin("Character Debugging");
		if (ImGui::SliderFloat("Friction", &frictionValue, 0.0f, 1.0f)) // Adjust the range as needed
		{
			this->rigidBody->setFriction(frictionValue);
		}
		
		// Apply the updated friction value to the character's rigid body
	
		ImGui::SliderFloat("Force Factor", &forceFactor, 0.1f, 50.0f); // Adjust the range as needed
		ImGui::Text("Force: %.2f, %.2f, %.2f", force.x(), force.y(), force.z());
		
		if (!rigidBody->isActive()) {
			// Print a message to the console
			ImGui::Text("RigidBody is not active!");
			
		}
		ImGui::End();
	}
}

btRigidBody* Character::getRigidBody()
{
	if (this->rigidBody != nullptr) {
		return this->rigidBody;
	}
	else
	return nullptr;
}

void Character::applyMovement()
{

	//Factor force
	force *= forceFactor;
	if (characterActive)
	{
		rigidBody->applyCentralForce(force);
		//Debug to console
		//std::cout << "\t" << force.x() << ", " << force.y() << ", " << force.z();
		force = btVector3(0.0, 0.0, 0.0);
	}
}
