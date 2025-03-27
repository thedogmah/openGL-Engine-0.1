#pragma once
//#include <glm/glm.hpp> // Include any necessary headers for the variables

#define BT_USE_DOUBLE_PRECISION
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0])) //first element must always exist to use this
#define ASSIMP_LOAD_FLAG (   aiProcess_Triangulate  | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

#include "Cube.h"
#include <set>
#include <map>
#include <vector>
#include "c:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\btBulletDynamicsCommon.h"
#include "Lights.h"
#include <c:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <c:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\LinearMath/btVector3.h>
#include "Texture.h"
#include "Mesh.h"
#include <assimp/scene.h>
#include "ImGuiLogger.h"
#include "ObjImporter.h"
//#include <memory>  // For std::unique_ptr


struct MeshNew;
struct modelNew;

extern modelNew* activeModelPtr;

extern float deltaTimeGlobal;
//#include "Camera.h"
class ObjImporter;

//create some logging data types and structures
extern ImGuiLogger logger;
struct FramebufferObject {
    GLuint fbo;
    GLuint texColorBuffer;
    GLuint rbo; // Renderbuffer for depth and stencil
    int width;
    int height;
    std::string name;  // User-defined FBO name
    std::string textureName; // Auto-generated texture name

    bool customViewEnabled = false;  // If true, use custom view matrix
    glm::mat4 viewMatrix;  // Custom view matrix (copied from camera view matrix)

};

//extern std::map<int, FramebufferObject> framebuffers;
// Key could be window ID or any identifier


class Texture;
//assimp scenes (AIscene)
extern std::vector<const aiScene*>scenes;
extern bool boolToolResized; // if mouse wheel is scrolled this turns true, then if terrrain edit mode is also on, tool will change ize visually, then this bool go bk to fals
extern bool terrainLMouseClicked;
extern float globalTime;
extern bool boolShowGLErrors;
extern bool boolDrawUI; //bool for drawing main UI in main.cpp (left Alt key on and off)
extern bool lMouseClicked;
extern bool UIdrawn;
extern bool loadModelTwo;
extern std::vector<ObjImporter> models;
//Create the game character
class Character;
extern float characterCameraXOffset;
extern float characterCameraYOffset;
extern float characterCameraZOffset;
class Camera;
extern Camera camera;
extern bool drawIMGUI;  
extern bool boolShowGrid;
extern bool boolShowWater;
class SSBO;
extern glm::mat4 cubeModelMatrix;

extern glm::mat4 model;
extern glm::mat4 view;// = glm::lookAt(glm::vec3(0.5f, -0.7f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
extern glm::mat4 projection;// = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 400.0f);

//no instances gnerates in water bounds for land objects
class Mesh;
extern GLuint* defaultShaderProgramPtr;

extern GLuint* globalWaterShader;
extern int modelLoc ;
extern int viewLoc ;
extern int projectionLoc ;
inline extern int selectedOption = 1;
inline extern bool wireframe = false;
extern 	GLuint terrainPickingSwitch;
//animation resources
struct AnimationData;
extern std::vector<AnimationData> animationsNew;
class AnimationController;
extern AnimationController* animationControllerPtr;


inline extern float window_height= 1200, window_width= 2400;
inline extern int mousePickingValue = 0;
//inline extern bool lMouseClicked= false;
//Sets created for debugging locations.
//inline extern std::set<btVector3> setDrawLinePositions;
 extern std::set<glm::dvec3> setCubePositions;
 struct Sun {

     float Brightness;
     glm::vec3 DiffuseColor;
     float radiansTime;
     glm::vec3 Pos;
     bool movingForward = true; // this boolean is used in the main.cpp engine to keep the sun moving constantly and
     //reversing back on itself.
 };

 extern Sun sun;

struct Vertex {
    float x, y, z;       // Vertex coordinates
    float u, v;          // Texture coordinates
    float nx, ny, nz;    // Normal coordinates
    float r, g, b, a ;
  
};
extern 	int brushSize;

// Enum for tool selection
enum class Tool {
    NONE,
    TERRAIN_SCULPT,
    PAINT,
    ERASE,
    // Add other tools as needed
};

// Global variable for the current tool
extern Tool currentTool;

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

// Vertex Shader source code as a const char* string


