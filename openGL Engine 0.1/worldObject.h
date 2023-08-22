#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

class worldObject
{
public:
	worldObject();
	glm::vec3 objectColour;
	unsigned int ID;
	float paddng{};
	glm::vec3 objectRandomColour;
	static unsigned int nextID;
	unsigned int returnID();
	//	unsigned int returnColor();

};

	
static_assert(sizeof(worldObject) % 16 == 0, "worldObject size not aligned");
