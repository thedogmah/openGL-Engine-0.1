#pragma once
#include <glm/glm.hpp>
#include "worldObject.h"
namespace World {
	class Lights
	{
	public:
		Lights();

		void update();

	public:
		glm::vec3 position;
		float strength;
		glm::vec3 colour;
		float pad1{};
		glm::vec3 startingPosition;
		float pad2{};
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
		float padding{};
		//glm::vec3 colour;
		//float padding2{};

	};

};

static_assert(sizeof(World::cubeInstance) % 16 == 0, "worldObject size not aligned");
