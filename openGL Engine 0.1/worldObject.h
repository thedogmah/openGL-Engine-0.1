#pragma once

#define BT_USE_DOUBLE_PRECISION
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include "c:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\btBulletDynamicsCommon.h"

#include <c:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\BulletCollision/CollisionDispatch/btCollisionWorld.h>

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
	

};

	
static_assert(sizeof(worldObject) % 16 == 0, "worldObject size not aligned");
