#pragma once
//#include <glm/glm.hpp> // Include any necessary headers for the variables

#define BT_USE_DOUBLE_PRECISION
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0])) //first element must always exist to use this
#define ASSIMP_LOAD_FLAG (   aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

#include "Cube.h"
#include <set>
#include <map>
#include <vector>
#include "btBulletDynamicsCommon.h"
#include "Lights.h"
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <LinearMath/btVector3.h>
#include "Texture.h"
#include "Mesh.h"

//#include "Camera.h"

class Texture;

extern bool boolShowGLErrors;
extern bool boolDrawUI; //bool for drawing main UI in main.cpp (left Alt key on and off)
extern bool lMouseClicked;
//Create the game character
class Character;
extern float characterCameraXOffset;
extern float characterCameraYOffset;
extern float characterCameraZOffset;
class Camera;
extern Camera camera;
extern bool drawIMGUI;
class SSBO;
extern glm::mat4 cubeModelMatrix;

extern glm::mat4 model;
extern glm::mat4 view;// = glm::lookAt(glm::vec3(0.5f, -0.7f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
extern glm::mat4 projection;// = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 400.0f);

class Mesh;
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


struct customShader{

    GLuint shaderProgramID;
    std::string vertexShaderCode;
    std::string fragmentShaderCode;
    std::string shaderName;
};

//CUSTOM SHADER IMGUI TOOLS BEGIN
extern std::string customVertexShaderCode;
extern std::string customFragmentShaderCode;

extern  std::vector<customShader*> customShaders;//custom shader vector assigned to each model uniquely.
//CUSTOM SHADER IMGUI TOOLS END


//two containers that map together
extern std::vector<SSBO*> SSBOVector;
extern std::vector<World::meshInstance*> meshInstancesVector;//container for all the vectors of unique dyanmic sSBOs (Below)
static std::vector<Mesh*> instancedMeshVector;//for importing multiple meshes for single SSBOs (grass / instances of trees etc)


static std::vector<Mesh*> meshVector;//for importing multiple meshes


extern Character* character;

extern std::map<SSBO, std::vector<World::cubeInstance>> mapSSBOMeshInstanceVector;

//OpenGL varaibles - also used by callback functions in callbackfunctions.h

//#endif // GLOBALS_H

//extern ImVec4 bgColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);     // Default background color
//extern ImVec4 buttonColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Default button color
//
//Ground Object.

//physics.
inline extern btDiscreteDynamicsWorld* dynamicsWorldPtr = nullptr;
extern 	btAlignedObjectArray<btCollisionShape*> collisionShapes;
extern std::vector<btRigidBody*> rigidBodies;
extern btRigidBody* groundBody;
extern 	GLuint modelMatrixLocation;
extern SSBO cubeSSBO;