#pragma once
//#include <glm/glm.hpp> // Include any necessary headers for the variables

#define BT_USE_DOUBLE_PRECISION
#include "Cube.h"
#include <set>

#include "btBulletDynamicsCommon.h"

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <LinearMath/btVector3.h>
inline extern int selectedOption = 1;
inline extern bool wireframe = false;

inline extern float window_height= 800, window_width= 1400;
inline extern int mousePickingValue = 0;
inline extern bool lMouseClicked= false;
//Sets created for debugging locations.
//inline extern std::set<btVector3> setDrawLinePositions;
 extern std::set<glm::dvec3> setCubePositions;


struct Vertex {
    float x, y, z;       // Vertex coordinates
    float u, v;          // Texture coordinates
    float nx, ny, nz;    // Normal coordinates
    float r, g, b, a ;
    // Color (red, green, blue, alpha)
};


//extern int screenWidth; // Declare global variables using 'extern' keyword
//extern int screenHeight;
//extern glm::vec3 cameraPosition;
//extern float globalScale;

//#endif // GLOBALS_H


//Ground Object.
