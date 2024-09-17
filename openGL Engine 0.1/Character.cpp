#include "Character.h"

Character::Character(btDynamicsWorld* world, float initialX, float initialY, float initialZ, GLFWwindow* window)
{
	btCollisionShape* characterShape = new btCapsuleShape(1, 1.0);
	this->window = window;
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(33.0, 33.0, 10.0));

	//create mass for character
	btScalar mass = 2;
	btVector3 localInertia(0, 0, 0);
	characterShape->calculateLocalInertia(mass, localInertia);
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo CI(mass, motionState, characterShape, localInertia);
	rigidBody = new btRigidBody(CI);
	rigidBody->forceActivationState(ACTIVE_TAG);
	rigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	world->addRigidBody(rigidBody);

	

	// Update linear and angular damping each frame
	rigidBody->setDamping(linearDamping, angularDamping);


}

void Character::handleInput()
{
	glm::vec3 cameraDirection = camera.mFront; // Use the camera's facing direction

	if (characterActive) {
		// Apply input to the character's velocity (force)
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			// Move forward in the camera's direction
			rigidBody->forceActivationState(ACTIVE_TAG);
			force += cameraDirection;
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			// Move backward (opposite direction of the camera's view)
			rigidBody->forceActivationState(ACTIVE_TAG);
			force -= cameraDirection;
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			// Calculate a leftward direction relative to the camera's view
			rigidBody->forceActivationState(ACTIVE_TAG);
			glm::vec3 leftDirection = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection);
			force += leftDirection;
		}
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			// Calculate a rightward direction relative to the camera's view
			rigidBody->forceActivationState(ACTIVE_TAG);
			glm::vec3 rightDirection = glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
			force += rightDirection;
		}
		else {
		
		}
		// Normalize the force vector to maintain consistent movement speed
		//force = glm::normalize(force);
	}
}

void Character::update()
{
	ImGui::Begin("Character Debugging");


	// ImGui for setting location
	ImGui::Text("Set Location:");
	static float newLocation[3] = { rigidBody->getWorldTransform().getOrigin().getX(),
									rigidBody->getWorldTransform().getOrigin().getY(),
									rigidBody->getWorldTransform().getOrigin().getZ() };

	ImGui::InputFloat3("X, Y, Z", newLocation);

	if (ImGui::Button("Set Location"))
	{
		setLocation(newLocation[0], newLocation[1], newLocation[2]);
	}

	ImGui::End();

	handleInput();
	applyMovement();
	
	debug();
}

void Character::debug()
{
	if (ImGui::GetCurrentContext()) {
		if (drawIMGUI) {
			ImGui::Begin("Character Debugging");
			if (ImGui::SliderFloat("Friction", &frictionValue, 0.0f, 1.0f)) // Adjust the range as needed
			{
				this->rigidBody->setFriction(frictionValue);
			}
			if (ImGui::SliderFloat("Linear Damping", &linearDamping, 0.01f, 5.0f)) {
				rigidBody->setDamping(linearDamping, angularDamping);
			}
			if (ImGui::SliderFloat("Angular Damping", &angularDamping, 0.01f, 5.0f)) {
				rigidBody->setDamping(linearDamping, angularDamping);
			}
			// Apply the updated friction value to the character's rigid body

			ImGui::SliderFloat("Force Factor", &forceFactor, 0.1f, 50.0f); // Adjust the range as needed
			ImGui::Text("Force: %.2f, %.2f, %.2f", force.x, force.y, force.z);

			if (!rigidBody->isActive()) {
				// Print a message to the console
				ImGui::Text("RigidBody is not active!");

			}
			ImGui::End();
		}

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

void Character::setLocation(float x, float y, float z)
{
	btTransform newTransform;
	newTransform.setIdentity();
	newTransform.setOrigin(btVector3(x, y, z));
	rigidBody->setWorldTransform(newTransform);
}

void Character::applyMovement()
{
	rigidBody->forceActivationState(ACTIVE_TAG);
	//Factor force
	force *= forceFactor;
	if (characterActive)
	{
		rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
		//Debug to console
		//std::cout << "\tForce vec3: " << force.x << ", " << force.y << ", " << force.z;
		force = glm::vec3(0.0, 0.0, 0.0);
	}
}
