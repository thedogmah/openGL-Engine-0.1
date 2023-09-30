#pragma once
//#include <glm/glm.hpp> // Include any necessary headers for the variables

#define BT_USE_DOUBLE_PRECISION
#include "Cube.h"
#include <set>

#include "btBulletDynamicsCommon.h"

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <LinearMath/btVector3.h>


extern bool lMouseClicked;
//Create the game character
class Character;
class Camera;
extern Camera camera;
class SSBO;
extern glm::mat4 cubeModelMatrix;

extern glm::mat4 model;
extern glm::mat4 view;// = glm::lookAt(glm::vec3(0.5f, -0.7f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
extern glm::mat4 projection;// = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 400.0f);




extern GLuint* defaultShaderProgramPtr;
extern int modelLoc ;
extern int viewLoc ;
extern int projectionLoc ;
inline extern int selectedOption = 1;
inline extern bool wireframe = false;

inline extern float window_height= 800, window_width= 1400;
inline extern int mousePickingValue = 0;
//inline extern bool lMouseClicked= false;
//Sets created for debugging locations.
//inline extern std::set<btVector3> setDrawLinePositions;
 extern std::set<glm::dvec3> setCubePositions;
 
struct Vertex {
    float x, y, z;       // Vertex coordinates
    float u, v;          // Texture coordinates
    float nx, ny, nz;    // Normal coordinates
    float r, g, b, a ;
  
};

inline extern Character* character;


//OpenGL varaibles - also used by callback functions in callbackfunctions.h

//#endif // GLOBALS_H


//Ground Object.

//physics.
inline extern btDiscreteDynamicsWorld* dynamicsWorldPtr = nullptr;
extern 	btAlignedObjectArray<btCollisionShape*> collisionShapes;
extern std::vector<btRigidBody*> rigidBodies;
extern btRigidBody* groundBody;
extern 	GLuint modelMatrixLocation;
extern SSBO cubeSSBO;