#pragma once
#include <glm/glm.hpp>
#include "worldObject.h"
#include "Sphere.h"
#include <glm/gtx/compatibility.hpp> 
namespace World {
	struct LightGPUData {
		glm::vec3 position;  // vec3 (12 bytes) -> Needs to be aligned to 16 bytes
		float strength;      // float (4 bytes)

		glm::vec3 colour;    // vec3 (12 bytes) -> Needs to be aligned to 16 bytes
		float pad1;          // Padding to align to 16 bytes

		glm::vec3 direction;  // vec3 (12 bytes) -> Needs to be aligned to 16 bytes
		float isDirectional =0.;                  // Padding to align to 16 bytes

		glm::vec3 startingPosition;
		float pad2;
	};
	struct lerpContainer {
		int currentPoint = 0;          // Current point index
		float currentLerpTime = 0.0f;  // Time spent in lerp
		float moveDuration = 5.0f;     // Duration to move between points
		float speedMultiplier = 0.5f;  // Speed multiplier
	};	
	class Lights {
	public:
		Lights();

		// Data that is sent to the GPU
		LightGPUData gpuData;  // Only contains position, strength, colour, etc.

		// Movement data (not sent to the GPU)
		lerpContainer lightLerp;            // Each light has its own lerp state
		std::vector<glm::vec3> curvePositions;  // Each light has its own curve positions

		// CPU-side function to update position based on lerp
		void update(float deltaTime);
	};

	struct Material {
	public:
		glm::vec3 ambient;
		float shininess;
		glm::vec3 diffuse;
		float transparency;
		glm::vec3 specular;
		float pad1;
		};

	struct cubeInstance : public worldObject {

		glm::mat4 modelMatrix;
		glm::vec3 rotationPerFrame;
		glm::uvec2 padding;
		btRigidBody* rigidBody = nullptr;
		//float padding[1];
		
		//glm::vec3 colour;
	//	float padding2{};

	};

	struct meshInstance : public worldObject {

		glm::mat4 modelMatrix;
		glm::vec3 rotationPerFrame;
		glm::uvec2 padding;
		btRigidBody* rigidBody = nullptr;
		// Add any other relevant properties for the specific mesh type
	};

};

static_assert(sizeof(World::cubeInstance) % 16 == 0, "worldObject size not aligned");


static_assert(sizeof(World::meshInstance) % 16 == 0, "worldObject size not aligned");

