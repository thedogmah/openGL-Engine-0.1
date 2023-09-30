#define _CRT_SECURE_NO_WARNINGS 
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define BT_USE_DOUBLE_PRECISION
#include "stb_image_write.h"
#include <stb/stb_image.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "globals.h"
#include "CallbackFunctions.h"
#include <random>
#include <iostream>
#include <istream>
#include "btBulletDynamicsCommon.h"

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "worldObject.h"
#include "cubeData.h"
#include "3D.h"
#include "Cube.h"
#include "Lights.h"
#include "SSBO.h"
#include "openGLDrawable.h"
#include "PhysDebug.h"
#include "Engine.h"
#include "Character.h"

// Declare and define the global variables
glm::mat4 cubeModelMatrix;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::lookAt(glm::vec3(0.5f, -0.7f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 30000.0f);
GLuint* defaultShaderProgramPtr = new GLuint;
btRigidBody* groundBody;
int modelLoc;
int viewLoc;
int projectionLoc;
SSBO cubeSSBO;
std::vector<btRigidBody*> rigidBodies;
btAlignedObjectArray<btCollisionShape*> collisionShapes;
bool lMouseClicked = false;
Camera camera;
GLuint modelMatrixLocation = 0;
Character* character = nullptr;


//forward declarations
void initialise(float x, float y, float z, GLFWwindow* window);
//function for returning shape type as string
std::string GetShapeTypeString(int shapeType);

void update();//Update character and world data / Also main function for other update functions

struct Mesh {
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};

//pointer to dynamics world and a vector containing rigid bodies to keep track of
// keep in mind dynamicsWorld also stores the rigid body for the simulation
std::vector<btRigidBody*>* rigidBodyVectorPtr = nullptr;
std::vector<Cube> instVector;

//Terrain and Noise Generation Varaibles
Terrain terrain;
//Must generate dynamicsworld ptr for terrain also
glm::vec2 Grid(10, 10); //slider for grid visualisation size in imgui window 
bool boolDrawHeightMap = true;
float groundPositionY = 1.0f;
std::vector<float> instCubeVertices = {
	// front
	-1.0, -1.0,  1.0,
	 1.0, -1.0,  1.0,
	 1.0,  1.0,  1.0,
	-1.0,  1.0,  1.0,
	// back
	-1.0, -1.0, -1.0,
	 1.0, -1.0, -1.0,
	 1.0,  1.0, -1.0,
	-1.0,  1.0, -1.0
};
//ground object
float quadGroundVertices[] = {
	// Position          // Texture Coordinates
	-0.5f, 0.0f, -0.5f, 0.0f, 0.0f,
	 0.5f, 0.0f, -0.5f, 1.0f, 0.0f,
	-0.5f, 0.0f,  0.5f, 0.0f, 1.0f,
	 0.5f, 0.0f,  0.5f, 1.0f, 1.0f,
};

std::vector<unsigned int> instCubeIndices = {
	// front face
	0, 1, 2,
	2, 3, 0,
	// back face
	4, 5, 6,
	6, 7, 4,
	// left face
	4, 7, 3,
	3, 0, 4,
	// right face
	1, 5, 6,
	6, 2, 1,
	// top face
	3, 2, 6,
	6, 7, 3,
	// bottom face
	0, 1, 5,
	5, 4, 0
};



void drawUI();
void instanceCubeFunction(GLuint VAO, GLuint VBO, GLuint EBO);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void addCubes(GLuint shader, Mesh mesh);
Cube* findCubeByColour(const glm::vec3 color); //used to access teh cubes properties or ptr to cube
/*int findCubeIndexByColour(const glm::vec3 color);*/ //used to delete items from vector
//SSBO* cubeSSBOptr= nullptr;
int cubesToGenerate; //for IMGUI input
glm::mat4 createCamera(glm::vec3& cameraPosition, glm::vec3& targetPosition, glm::vec3& upVector);


int main()
{

	
	int i;
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorldPtr = dynamicsWorld;
	
	dynamicsWorld->setGravity(btVector3(0, -8.81, 0));

	///-----initialization_end-----

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	
	cubeModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
	///create a few basic rigid bodies

	// 	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	
	 groundBody = nullptr;

	// SSBO cubeSSBO;

	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(500.), btScalar(0.1), btScalar(500.)));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, 0, 0));

		btScalar mass(0.0);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		groundBody = body;
		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}
	
	{
		

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(2.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(1, 5, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//dynamicsWorld->addRigidBody(body);

	}
	{


		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(2.f));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(0.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(1,5, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	/*	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);*/
		btCollisionObject* staticSphere = new btCollisionObject();
		staticSphere->setCollisionShape(colShape);
		staticSphere->setWorldTransform(startTransform);
		//dynamicsWorld->addCollisionObject(staticSphere);

	}




	
	

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	GLFWmonitor* monitor = monitors[0];
	GLFWwindow* window = glfwCreateWindow(1400, 800, "Shape Engine",NULL, NULL);
	
	//GLFWmonitor 
	//int width, height;
	//glfwGetFramebufferSize(window, &width, &height);
//	glViewport(0, 0, width, height);
//	std::cout << width << "," << height;
	if (window == NULL)
	{

		std::cout << "Didn't create window";
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to init GLAD" << std::endl;
		return 0;
	}
	int frameBufferWidth, frameBufferHeight;
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	glViewport(0, 0, frameBufferWidth, frameBufferHeight);

	// resizes viewport based on automatic resize, passes parameters to the gl viewport

	float initialTime= glfwGetTime();
	float time = initialTime;
	//registers to GLFW the function,


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = static_cast<float>(window_width);
	io.DisplaySize.y = static_cast<float>(window_height);

	//ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	//GL Drawer debugger
	PhysDebug debugger;
	
	terrain.shaderPtr = debugger.shaderPtr;
	terrain.dynamicsWorldUniversalPtr = dynamicsWorld;
	//debugger.setDebugMode(btIDebugDraw::DBG_DrawConstraints);  // Draw wireframes of objects
	dynamicsWorld->setDebugDrawer(&debugger);
	debugger.setDebugMode(btIDebugDraw::DBG_DrawWireframe);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debugCallback, nullptr);
	//std::vector<float> pyramidVertices = {
	//	// Base vertices (triangle 1)
	//	-0.5f, 0.0f, -0.5f, // Vertex 0
	//	0.5f, 0.0f, -0.5f,  // Vertex 1
	//	0.5f, 0.0f, 0.5f,   // Vertex 2

	//	// Base vertices (triangle 2)
	//	0.5f, 0.0f, 0.5f,   // Vertex 2
	//	-0.5f, 0.0f, 0.5f,  // Vertex 3
	//	-0.5f, 0.0f, -0.5f, // Vertex 0

	//	// Side faces (triangles)
	//	-0.5f, 0.0f, -0.5f, // Vertex 0
	//	0.0f, 1.0f, 0.0f,   // Vertex 4
	//	0.5f, 0.0f, -0.5f,  // Vertex 1

	//	0.5f, 0.0f, -0.5f,  // Vertex 1
	//	0.0f, 1.0f, 0.0f,   // Vertex 4
	//	0.5f, 0.0f, 0.5f,   // Vertex 2

	//	0.5f, 0.0f, 0.5f,   // Vertex 2
	//	0.0f, 1.0f, 0.0f,   // Vertex 4
	//	-0.5f, 0.0f, 0.5f,  // Vertex 3

	//	-0.5f, 0.0f, 0.5f,  // Vertex 3
	//	0.0f, 1.0f, 0.0f,   // Vertex 4
	//	-0.5f, 0.0f, -0.5f, // Vertex 0
	//};

	//std::vector<float> pyramidNormals= {  // Base normals (triangle 1)
	//0.0f, 1.0f, 0.0f, // Normal 0
	//0.0f, 1.0f, 0.0f, // Normal 1
	//0.0f, 1.0f, 0.0f, // Normal 2

	//// Base normals (triangle 2)
	//0.0f, 1.0f, 0.0f, // Normal 2
	//0.0f, 1.0f, 0.0f, // Normal 3
	//0.0f, 1.0f, 0.0f, // Normal 0

	//// Side normals (triangles)
	//0.0f, 0.0f, -1.0f, // Normal 0
	//0.0f, 0.0f, -1.0f, // Normal 1
	//0.0f, 0.0f, -1.0f, // Normal 4

	//1.0f, 0.0f, 0.0f, // Normal 1
	//1.0f, 0.0f, 0.0f, // Normal 2
	//1.0f, 0.0f, 0.0f, // Normal 4

	//0.0f, 0.0f, 1.0f, // Normal 2
	//0.0f, 0.0f, 1.0f, // Normal 3
	//0.0f, 0.0f, 1.0f, // Normal 4

	//-1.0f, 0.0f, 0.0f, // Normal 3
	//-1.0f, 0.0f, 0.0f, // Normal 0
	//-1.0f, 0.0f, 0.0f, // Normal 4
	//};

	std::vector<float> cubeVertices = {
		// Front		//tex coords
	 -0.5, -0.5,  0.5,  0.0, 0.0, // Bottom left
	 0.5, -0.5,  0.5,  1.0, 0.0, // Bottom right
	 0.5,  0.5,  0.5,  1.0, 1.0, // Top right

	 0.5,  0.5,  0.5,  1.0, 1.0, // Top right
	-0.5,  0.5,  0.5,  0.0, 1.0, // Top left
	-0.5, -0.5,  0.5,  0.0, 0.0, // Bottom left

	// Back face
	-0.5, -0.5, -0.5,  1.0, 0.0, // Bottom right
	 0.5, -0.5, -0.5,  0.0, 0.0, // Bottom left
	 0.5,  0.5, -0.5,  0.0, 1.0, // Top left

	 0.5,  0.5, -0.5,  0.0, 1.0, // Top left
	-0.5,  0.5, -0.5,  1.0, 1.0, // Top right
	-0.5, -0.5, -0.5,  1.0, 0.0, // Bottom right

	// Right face
	 0.5, -0.5, -0.5,  0.0, 0.0, // Bottom left
	 0.5, -0.5,  0.5,  1.0, 0.0, // Bottom right
	 0.5,  0.5,  0.5,  1.0, 1.0, // Top right

	 0.5,  0.5,  0.5,  1.0, 1.0, // Top right
	 0.5,  0.5, -0.5,  0.0, 1.0, // Top left
	 0.5, -0.5, -0.5,  0.0, 0.0, // Bottom left

	 // Left face
	 -0.5, -0.5,  0.5,  1.0, 0.0, // Bottom left
	 -0.5, -0.5, -0.5,  0.0, 0.0, // Bottom right
	 -0.5,  0.5, -0.5,  0.0, 1.0, // Top right

	 -0.5,  0.5, -0.5,  0.0, 1.0, // Top right
	 -0.5,  0.5,  0.5,  1.0, 1.0, // Top left
	 -0.5, -0.5,  0.5,  1.0, 0.0, // Bottom left

	 // Top face
	 -0.5,  0.5,  0.5,  0.0, 0.0, // Bottom left
	  0.5,  0.5,  0.5,  1.0, 0.0, // Bottom right
	  0.5,  0.5, -0.5,  1.0, 1.0, // Top right

	  0.5,  0.5, -0.5,  1.0, 1.0, // Top right
	 -0.5,  0.5, -0.5,  0.0, 1.0, // Top left
	 -0.5,  0.5,  0.5,  0.0, 0.0, // Bottom left

	 // Bottom face
	 -0.5, -0.5, -0.5,  0.0, 0.0, // Bottom left
	  0.5, -0.5, -0.5,  1.0, 0.0, // Bottom right
	  0.5, -0.5,  0.5,  1.0, 1.0, // Top right

	  0.5, -0.5,  0.5,  1.0, 1.0, // Top right
	 -0.5, -0.5,  0.5,  0.0, 1.0, // Top left
	 -0.5, -0.5, -0.5,  0.0, 0.0  // Bottom left
	};


	std::vector<float> cubeVerticesPure = {
		// Front
	 -0.5, -0.5,  0.5,   // Bottom left
	 0.5, -0.5,  0.5,   // Bottom right
	 0.5,  0.5,  0.5,   // Top right

	 0.5,  0.5,  0.5,  // Top right
	-0.5,  0.5,  0.5,   // Top left
	-0.5, -0.5,  0.5,   // Bottom left

	// Back face
	-0.5, -0.5, -0.5,   // Bottom right
	 0.5, -0.5, -0.5,   // Bottom left
	 0.5,  0.5, -0.5,   // Top left

	 0.5,  0.5, -0.5,   // Top left
	-0.5,  0.5, -0.5,   // Top right
	-0.5, -0.5, -0.5,   // Bottom right

	// Right face
	 0.5, -0.5, -0.5,  // Bottom left
	 0.5, -0.5,  0.5,   // Bottom right
	 0.5,  0.5,  0.5,   // Top right

	 0.5,  0.5,  0.5,  // Top right
	 0.5,  0.5, -0.5,   // Top left
	 0.5, -0.5, -0.5,   // Bottom left

	 // Left face
	 -0.5, -0.5,  0.5,  // Bottom left
	 -0.5, -0.5, -0.5,   // Bottom right
	 -0.5,  0.5, -0.5,   // Top right

	 -0.5,  0.5, -0.5,   // Top right
	 -0.5,  0.5,  0.5,  // Top left
	 -0.5, -0.5,  0.5,   // Bottom left

	 // Top face
	 -0.5,  0.5,  0.5,   // Bottom left
	  0.5,  0.5,  0.5,   // Bottom right
	  0.5,  0.5, -0.5,   // Top right

	  0.5,  0.5, -0.5,   // Top right
	 -0.5,  0.5, -0.5,  // Top left
	 -0.5,  0.5,  0.5,   // Bottom left

	 // Bottom face
	 -0.5, -0.5, -0.5,   // Bottom left
	  0.5, -0.5, -0.5,  // Bottom right
	  0.5, -0.5,  0.5,   // Top right

	  0.5, -0.5,  0.5,   // Top right
	 -0.5, -0.5,  0.5,   // Top left
	 -0.5, -0.5, -0.5 // Bottom left
	};

	std::vector<unsigned int> cubeIndices = {
		// front face
	 0, 1, 2,
	3, 4, 5,

	// Back face
	6, 7, 8,
	9, 10, 11,

	// Right face
	12, 13, 14,
	15, 16, 17,

	// Left face
	18, 19, 20,
	21, 22, 23,

	// Top face
	24, 25, 26,
	27, 28, 29,

	// Bottom face
	30, 31, 32,
	33, 34, 35
	};

	std::vector<glm::vec3> cubeNormals = {
		// Front face
   glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),

	// Back face
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),

	// Right face
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),

	// Left face
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),

	// Top face
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),

	// Bottom face
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	};

	for (glm::vec3& normal : cubeNormals) {
		normal = glm::normalize(normal);
	}
	stbi_set_flip_vertically_on_load(true);
	int imgWidth, imgHeight, colChannels;
	unsigned char* imgData = stbi_load("me.jpg", &imgWidth, &imgHeight, &colChannels, 3);
	////std::cout << imgData;

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texture parameters (filtering and wrapping)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
	}
	// Upload image data to the texture
	if (imgData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Free the image data after uploading
		stbi_image_free(imgData);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}


	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * cubeIndices.size(), cubeIndices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * cubeVertices.size()) + (3 * sizeof(float) * cubeNormals.size()), NULL, GL_STATIC_DRAW);
	// Upload vertex data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * cubeVertices.size(), cubeVertices.data());
	// Upload normals data
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * cubeVertices.size(), 3 * sizeof(float) * cubeNormals.size(), cubeNormals.data());

	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)* cubeVertices.size(), sizeof(glm::vec3)* normalsVertices.size(), normalsVertices.data());
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after setting up VAO attributes: " << error << std::endl;

	}


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * cubeVertices.size()));
	//	glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));



		//std::vector<float> verticesPlain  =
		//	{ //     COORDINATES     /        Texture      /   TexCoord  //
		//		-0.5f, -0.5f, 0.0f,    	0.0f, 0.0f, // Lower left corner
		//		-0.5f,  0.5f, 0.0f,    	0.0f, 1.0f, // Upper left corner
		//		 0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // Upper right corner
		//		 0.5f, -0.5f, 0.0f,    	1.0f, 0.0f  // Lower right corner
		//	};

	glBindTexture(GL_TEXTURE_2D, 0);
	//std::vector<unsigned int> indicesPlain = {
	//	0, 2, 1, // Upper 
	//	0, 3, 2 // 
	//	};

	////pyramid verices

	//std::vector<float> normalsPlain = {
	//	0.0f, 0.0f, 1.0f
	//	};

	// Unbind VAO
	//glBindVertexArray(0);

	//Load shaders into memory
	std::ifstream vertShaderFile("basic.vert");
	if (!vertShaderFile.is_open())

	{
		std::cout << "Basic.vert not opened";
	}

	std::ifstream fragShaderFile("basic.frag");
	if (!fragShaderFile.is_open())

	{
		std::cout << "Basic.frag not opened";
	}


	glGenFramebuffers(1, &colorFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);


	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after setting up FBO textures: " << error << std::endl;

	}

	glGenRenderbuffers(1, &depthrenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Frame buffer not complete";
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	//create string stream for file contents then put new contents into string. 
	std::stringstream vertexStringStream;
	vertexStringStream << vertShaderFile.rdbuf();
	const std::string vertexString = vertexStringStream.str();
	//new string copied over into a pointer to char to pass to the function.
	const char* vertexSourcePtr = vertexString.c_str();


	unsigned int basicVertex;
	basicVertex = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(basicVertex, 1, &vertexSourcePtr, 0);
	glCompileShader(basicVertex);

	int  success;
	char infoLog[512];
	glGetShaderiv(basicVertex, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(basicVertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	else {

		std::cout << "Vertex shader compiled successfully";

	}

	std::stringstream fragStringStream;
	fragStringStream << fragShaderFile.rdbuf();
	const std::string fragString = fragStringStream.str();

	const char* fragSourcePtr = fragString.c_str();

	unsigned int basicFragment;
	basicFragment = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(basicFragment, 1, &fragSourcePtr, 0);
	glCompileShader(basicFragment);

	glGetShaderiv(basicFragment, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(basicFragment, 512, NULL, infoLog);
		std::cout << "\nERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	else {

		std::cout << "\nFragment shader compiled successfully";

	}
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	
	glAttachShader(shaderProgram, basicVertex);
	glAttachShader(shaderProgram, basicFragment);
	glLinkProgram(shaderProgram);


	glUseProgram(shaderProgram);
	*defaultShaderProgramPtr = shaderProgram;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error afterpolygon mode " << error << std::endl;

	}
	glClearColor(0.1, 0.1, 0.2, 0.0);
	glEnable(GL_DEPTH_TEST);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after enabling depth test: " << error << std::endl;

	}


	int textureUniformLoc = glGetUniformLocation(shaderProgram, "texture1");
	glUniform1i(textureUniformLoc, 0); // 0 corresponds to GL_TEXTURE0

	float rotation = 0.15;
	//	glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after enabling glblend: " << error << std::endl;

	}


	//array of structs testing for drawing openGLDrawables


	std::vector<Vertex> cubeVerticesB = {
	{-0.5, -0.5,  0.5,  0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0},
	{0.5, -0.5,  0.5,  1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0},
	{0.5,  0.5,  0.5,  1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0},
	{0.5,  0.5,  0.5,  1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0},
	{-0.5,  0.5,  0.5,  0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0},
	{-0.5, -0.5,  0.5,  0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0},
 
	{ -0.5, -0.5, -0.5,  1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0 },
	{ 0.5, -0.5, -0.5,  0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0 },
	{ 0.5,  0.5, -0.5,  0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0 },
	{ 0.5,  0.5, -0.5,  0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0 },
	{ -0.5,  0.5, -0.5,  1.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0 },
	{-0.5, -0.5, -0.5,  1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0},

	{ 0.5, -0.5, -0.5,  0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 },
	{ 0.5, -0.5,  0.5,  1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 },
	{ 0.5,  0.5,  0.5,  1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0 },
	{ 0.5,  0.5,  0.5,  1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0 },
	{ 0.5,  0.5, -0.5,  0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 },
	{ 0.5, -0.5, -0.5,  0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 },	

	{ -0.5, -0.5,  0.5,  1.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0 },
	{-0.5, -0.5, -0.5,  0.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0},
	{-0.5,  0.5, -0.5,  0.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0},
	{-0.5,  0.5, -0.5,  0.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0},
	{-0.5,  0.5,  0.5,  1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0},
	{-0.5, -0.5,  0.5,  1.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0},

	{-0.5,  0.5,  0.5,  0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0},
	{ 0.5,  0.5,  0.5,  1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0},
	{0.5,  0.5, -0.5,  1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0},
	{ 0.5,  0.5, -0.5,  1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0},
	{-0.5,  0.5, -0.5,  0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0},
	{-0.5,  0.5,  0.5,  0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0},

	{-0.5, -0.5, -0.5,  0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
	{ 0.5, -0.5, -0.5,  1.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0, 0.0},
	{ 0.5, -0.5,  0.5,  1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
	{ 0.5, -0.5,  0.5,  1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
	{-0.5, -0.5,  0.5,  0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
	{-0.5, -0.5, -0.5,  0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0}
	
	};

	// Populate the cubeVertices vector
	
	

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error before openGL drawable constructor: " << error << std::endl;
	}
	openGLDrawable cubeTest(cubeVerticesB);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after openGL drawable constructor: " << error << std::endl;
	}
	std::vector<World::Lights> sceneLights;
	//Generate Lights
	World::Lights sun;
	World::Lights lamp1;
	World::Lights stars;
	sun.position = glm::vec3(2.0, 2.0, 1.0); // Example position, adjust as needed
	sun.colour = glm::vec3(0.3, 0.1,0.3);   // Example color, adjust as needed
	sun.strength = 0.5;    
	// Example strength, adjust as needed
	sun.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
	lamp1.position = glm::vec3(1.0, -3.0, 2.0); // Example position, adjust as needed
	lamp1.colour = glm::vec3(1.0, 0.1, 0.0);   // Example color, adjust as needed
	lamp1.strength =0.5;                 // Example strength, adjust as needed
	lamp1.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
	stars.position = glm::vec3(1.0, -3.0, 2.0); // Example position, adjust as needed
	stars.colour = glm::vec3(1.0, 1.0, 0.0);   // Example color, adjust as needed
	stars.strength = 0.5;
	stars.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
	World::Material material;
	material.ambient = glm::vec3(0.2, 0.2, 0.2); // Example ambient color, adjust as needed
	material.diffuse = glm::vec3(0.5, 0.5, 0.5); // Example diffuse color, adjust as needed
	material.specular = glm::vec3(0.8, 0.8, 0.8); // Example specular color, adjust as needed
	material.shininess =1.0;             // Example shininess, adjust as needed
	material.transparency = 0.75;
	// Get the location of the Material uniform in the shader program

	sceneLights.push_back(sun);
	sceneLights.push_back(stars);
	sceneLights.push_back(lamp1);
	
	//sceneLights.push_back(stars);
	// Upload the sceneLights data to the shader
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error before binding ssbos: " << error << std::endl;
	}
	SSBO ssboLighting(0, sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size(), GL_DYNAMIC_DRAW);
	SSBO ssboMaterial(1, &material, sizeof(material), GL_DYNAMIC_DRAW);
	 cubeSSBO = SSBO(2, cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size(), GL_DYNAMIC_DRAW);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after binding ssbos: " << error << std::endl;
		}

	//int arraySize = 4;
	//int araySizeLocation = glGetUniformLocation(shaderProgram, "arraySize");
	//initialise new light vectors for imgui
	World::Lights newLight;
	glm::vec3 positionNL{ glm::vec3(0.1,0.1,0.1) };
	glm::vec3 colourNL{ glm::vec3(0.0,1.0,0.5) };
	float strengthNL{ 0.2 };
	float sceneExposure{ 0.5 };

	//Generate 100 cube instances
	std::cout << "\size of ptr to rigid body is: " << sizeof(btRigidBody*) << "float: " << sizeof(float);
	std::cout << "\nsize of CubeInstance is:" << sizeof(World::cubeInstance);
	std::cout << "\nsize of World Object is:" << sizeof(worldObject);
	std::cout << "\nsize of GLM mat4:" << sizeof(glm::mat4);
	std::cout << "\nsize of GLM vec3:" << sizeof(glm::vec3);
	std::cout << "\nunsigned int:" << sizeof(unsigned int);
	
	rigidBodyVectorPtr = &rigidBodies;
	//for (int i = 0; i < 50; i++) {

	//	glm::dvec3 instancePosition = glm::dvec3(glm::linearRand(glm::vec3(0.0f), glm::vec3(50.0f)));
	//	glm::dvec3 instanceRotation = glm::linearRand(glm::vec3(-1.0f), glm::vec3(30.0f));
	//	glm::dvec3 instanceScale = glm::linearRand(glm::vec3(1.0f), glm::vec3(1.0f));
	//	glm::dmat4 modelMatrix(1.0f);
	//	modelMatrix = glm::scale(modelMatrix, instanceScale);
	//	modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.x), glm::dvec3(1.0f, 0.0f, 0.0f));
	//	modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.y), glm::dvec3(0.0f, 1.0f, 0.0f));
	//	modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.z), glm::dvec3(0.0f, 0.0f, 1.0f));
	//	
	//	modelMatrix = glm::translate(modelMatrix, instancePosition);
	//	// Create cube data
	//	World::cubeInstance cube;
	//	cube.modelMatrix = modelMatrix;
	//	
	//	// Create rigid body data
	//	glm::dvec3 cubePosition = glm::dvec3(cube.modelMatrix[3]);
	//	glm::vec3 scale = glm::vec3(cube.modelMatrix[0][0], cube.modelMatrix[1][1], cube.modelMatrix[2][2]);

	//	btCollisionShape* boxShape = new btBoxShape(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));

	//	btTransform startTransform;
	//	startTransform.setIdentity();
	//	startTransform.setOrigin(btVector3(cubePosition.x, cubePosition.y, cubePosition.z));

	//	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
	//	btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, motionState, boxShape);
	//	btRigidBody* rigidBody = new btRigidBody(rbInfo);
	//	//cube.rigidBody = rigidBody;
	//	dynamicsWorld->addRigidBody(rigidBody);
	//	rigidBody->setUserPointer(reinterpret_cast<void*>(cube.ID));

	//	rigidBodies.push_back(rigidBody); // St
	//	cubesSSBOVector.push_back(cube);

	//}

	//btScalar cubeSideLength = 10.0f;

	//// Create a collision shape for the cube
	//btCollisionShape* cubeShape = new btBoxShape(btVector3(cubeSideLength * 0.5f, cubeSideLength * 0.5f, cubeSideLength * 0.5f));
	//
	//// Define random position, rotation, and scale for the cube
	//glm::vec3 instancePosition = glm::linearRand(glm::vec3(-33.0f), glm::vec3(33.0f));
	//glm::vec3 instanceRotation = glm::linearRand(glm::vec3(-180.0f), glm::vec3(180.0f));
	//glm::vec3 instanceScale = glm::linearRand(glm::vec3(1.0f), glm::vec3(1.0f));

	//// Create the model matrix for the cube
	//glm::mat4 modelMatrix(1.0f);
	//modelMatrix = glm::translate(modelMatrix, instancePosition);
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	//modelMatrix = glm::scale(modelMatrix, instanceScale);

	//// Set the initial transformation for the cube
	//btTransform cubeTransform;
	//cubeTransform.setIdentity();
	//cubeTransform.setOrigin(btVector3(instancePosition.x, instancePosition.y, instancePosition.z));

	//// Create rigid body for the cube
	//btScalar cubeMass = 1.0f;
	//bool isCubeDynamic = (cubeMass != 0.f);

	//btVector3 cubeLocalInertia(0, 0, 0);
	//if (isCubeDynamic)
	//	cubeShape->calculateLocalInertia(cubeMass, cubeLocalInertia);

	//btDefaultMotionState* cubeMotionState = new btDefaultMotionState(cubeTransform);
	//btRigidBody::btRigidBodyConstructionInfo cubeRbInfo(cubeMass, cubeMotionState, cubeShape, cubeLocalInertia);
	//btRigidBody* cubeBody = new btRigidBody(cubeRbInfo);

	//// Add the cube rigid body to the dynamics world
	//dynamicsWorld->addRigidBody(cubeBody);
	//collisionShapes.push_back(cubeShape);

	//// Create cube data
	//World::cubeInstance cube;
	//cube.modelMatrix = modelMatrix;
	/////cubesSSBOVector.push_back(cube);
	//
	//model matrix for unique ground cube to pass to vertex shader if statement
	
	modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelUniform");
	GLuint isInstancedBool =  glGetUniformLocation(shaderProgram, "isInstanced");


	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "before terrain " << error << std::endl;



	}


	
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	}
	//char mapFile[] = "256.raw";
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	}
	char map[] = "map.raw";
	terrain.loadHeightMap(map, 16);
	terrain.generateHeightMap();
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	}
	cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	}
	cubeSSBOptr = &cubeSSBO;
	float currentTime, deltaTime;

	//initalise game / variables and start location

	initialise(1.0, 2.0, 1.0, window);
	while (!glfwWindowShouldClose(window))
	{
		
		
		//Get frame time
		currentTime = glfwGetTime();
		// Calculate the elapsed time since the start of the loop
		deltaTime = currentTime - initialTime;
		initialTime = currentTime;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	
		update();
		// Assuming you have the rigid body's position in 'rigidBodyPosition'
		btVector3 rigidBodyPosition = character->getRigidBody()->getCenterOfMassPosition();

		// Calculate the view matrix to focus on the rigid body's position
		glm::mat4 characterViewMatrix = glm::lookAt(glm::vec3(rigidBodyPosition.x(), rigidBodyPosition.y(), rigidBodyPosition.z()),
			glm::vec3(rigidBodyPosition.x(), rigidBodyPosition.y(), rigidBodyPosition.z() + 3.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		
		//camera.mPosition = glm::vec3(rigidBodyPosition.x(), rigidBodyPosition.y(), rigidBodyPosition.z() + 3.0f); // Offset the camera

		// Calculate the view matrix
		//camera.mViewMatrix = glm::lookAt(camera.mPosition, glm::vec3(rigidBodyPosition.x(), rigidBodyPosition.y(), rigidBodyPosition.z()), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

		//Updates all update functions, including character
		//if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_E) == GLFW_REPEAT)

		//{
		//	cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(0.0f, 0.01f, 0.0f));
		//}

		//if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_C) == GLFW_REPEAT)

		//{
		//	cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(0.0f, -0.01f, 0.0f));
		//}
		//if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_V) == GLFW_REPEAT)

		//{
		//	cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(1.0f),glm::vec3(1.0f, 0.0f, 0.0f));
		//}
		//if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_B) == GLFW_REPEAT)

		//{
		//	cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//}
		//while ((error = glGetError()) != GL_NO_ERROR) {
		//	std::cout << "OpenGL Error at game loop start: " << error << std::endl;



		//}
		//if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_N) == GLFW_REPEAT)

		//{
		//	cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//}
		////bullet
		//  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeModelMatrix));

		

		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at game loop start: " << error << std::endl;



		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window_width, window_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	//redundant uniform? now 'view' for camera matrix, not viewpos
		GLuint viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos");

		// Set the viewPos uniform to the camera's position
		glm::vec3 cameraPosition = camera.mPosition; // Change this to your camera's position
		glUniform3fv(viewPosLocation, 1, glm::value_ptr(cameraPosition));

		
		glm::vec3 center(0.0f, 0.0f, 0.0f); // Center point of the circular motion
		float radius = 7.0f; // Radius of the circular path
		float rotationSpeed = 0.5f; // Rotation speed in radians per second

		for (int i = 0; i < sceneLights.size(); i++) {
			glm::vec3 initialPosition = sceneLights[i].startingPosition;

			float angle = rotationSpeed * time + i * (2.0f * glm::pi<float>() / sceneLights.size());

			float x = center.x + radius * glm::cos(angle);
			float y = center.y;
			float z = center.z + radius * glm::sin(angle);

			glm::vec3 newPosition(x, y, z);
			sceneLights[i].position = newPosition;
		}

		//float currentTime = glfwGetTime(); // Get current time in seconds
		//
		//float angle = rotationSpeed * time;

		//float cameraX = center.x + radius * cos(angle);
		//float cameraZ = center.z + radius * sin(angle);

		//camera.mPosition = glm::vec3(cameraX, camera.mPosition.y, cameraZ);
		//camera.setViewMatrix();
			ssboLighting.updateData(sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size());

		//glUseProgram(shaderProgram);
		camera.update();
		
	
		// Set up uniforms
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0, 1.0, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		// Bind the texture before drawing
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Set the uniform value to the texture unit you bound the texture to (e.g., GL_TEXTURE0)
		glUniform1i(textureUniformLoc, 0); // 0 corresponds to GL_TEXTURE0

	
		// Bind VAO and draw cube
		
		
		//glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0); // Unbind VAO

		// Render the UI on top of the scene

		//create variable names for new light object

		drawUI();
		int w, h;

		//glfwGetWindowSize(window, &w, &h);

		
	
		// Input box for the number of cubes
		

			

			ImGui::Begin("Camera Location");
			if (ImGui::Button("Snap to World Origin")) {
				glm::mat4 worldOrigin = glm::mat4(1.0f); // Identity matrix

				// Set the camera's view matrix to look at the world center (0, 0, 0)
				glm::vec3 worldCenter(0.0f, 0.0f, 0.0f); // Replace with your desired world center

				camera.mPosition = worldCenter;
				camera.mViewMatrix = glm::lookAt(camera.mPosition, worldCenter, glm::vec3(0.0f, 1.0f, 0.0f));

				// Update other camera properties as needed
				// For example, resetting pitch and yaw angles
				camera.mPitch = 0.0f;
				camera.mYaw = -90.0f;
				camera.update();

				// Update the view and projection matrices in your shader
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
				glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
			}
			// Get the view matrix from your camera
			glm::mat4 viewMatrix = camera.getViewMatrix();

			// Extract the camera's position from the view matrix
			glm::vec3 cameraPos = glm::vec3(viewMatrix[3]);

			// Round the coordinates to one decimal place
			cameraPos.x = round(cameraPos.x * 10.0f) / 10.0f;
			cameraPos.y = round(cameraPos.y * 10.0f) / 10.0f;
			cameraPos.z = round(cameraPos.z * 10.0f) / 10.0f;

			ImVec4 textColor(1.0f, 1.0f, 0.0f, 1.0f); // Yellow color (RGBA)

			// Display the camera's location with yellow text color
			ImGui::TextColored(textColor, "Camera Location: %.1f, %.1f, %.1f", cameraPos.x, cameraPos.y, cameraPos.z);

			// End ImGui frame
			ImGui::End();

			glm::mat4 matrix1 = camera.getViewMatrix();
			glm::mat4 matrix2 = camera.getViewMatrix();
			glm::mat4 matrix3 = camera.getViewMatrix();

			ImVec4 color1 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
			ImVec4 color2 = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
			ImVec4 color3 = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue

			// Open a new ImGui window to display matrices
			ImGui::Begin("Matrices");

			// Display matrix1 with a red color
			ImGui::TextColored(color1, "Matrix 1 - View:");

			// Display matrix1's content, row by row
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					ImGui::Text("%.*f", 3, matrix1[i][j]);
					if (j < 3) ImGui::SameLine(); // Add separator
				}
			}

			// Add a separator line
			ImGui::Separator();

			// Display matrix2 with a green color
			ImGui::TextColored(color2, "Matrix 2:");

			// Display matrix2's content, row by row
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					ImGui::Text("%.*f", 3, matrix2[i][j]);
					if (j < 3) ImGui::SameLine(); // Add separator
				}
			}

			// Add a separator line
			ImGui::Separator();

			// Display matrix3 with a blue color
			ImGui::TextColored(color3, "Matrix 3:");

			// Display matrix3's content, row by row
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					ImGui::Text("%.*f", 3, matrix3[i][j]);
					if (j < 3) ImGui::SameLine(); // Add separator
				}
			}

			ImGui::End(); // End the ImGui window for matrices

			//ImGui::Begin("Cube Properties");

			//// Iterate through cubesSSBOVector
			//for (size_t i = 0; i < cubesSSBOVector.size(); ++i) {
			//	ImGui::Text("Object %zu:", i);

			//	// Display the ID
			//	ImGui::Text("ID: %u", cubesSSBOVector[i].ID);

			//	// Display the model matrix (position, scale, etc.)
			//	ImGui::Text("Model Matrix:");

			//	// Extract and display the position
			//	glm::mat4 modelMatrix = cubesSSBOVector[i].modelMatrix;
			//	glm::dvec3 position(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
			//	ImGui::Text("   Position (x, y, z): %.*f, %.*f, %.*f",
			//		std::numeric_limits<double>::max_digits10, position.x,
			//		std::numeric_limits<double>::max_digits10, position.y,
			//		std::numeric_limits<double>::max_digits10, position.z);

			//	glm::vec3 scale;
			//	scale.x = glm::length(glm::vec3(modelMatrix[0])); // First column
			//	scale.y = glm::length(glm::vec3(modelMatrix[1])); // Second column
			//	scale.z = glm::length(glm::vec3(modelMatrix[2])); // Third column

			//	ImGui::Text("   Scale (x, y, z): %.*f, %.*f, %.*f",
			//		std::numeric_limits<double>::max_digits10, scale.x,
			//		std::numeric_limits<double>::max_digits10, scale.y,
			//		std::numeric_limits<double>::max_digits10, scale.z);
			//	// You can add more lines here to display other properties like scale, rotation, etc.

			//	// Add a separator between objects
			//	ImGui::Separator();
			//}

			// End the ImGui window
			//ImGui::End();
	//	This code iterates through all the collision objects in your dynamics world, checks if each object is a rigid body, and prints relevant information like position, linear velocity, and angular velocity to the console.If an object is not a rigid body, it prints "Unknown Collision Object Type." You can customize the output and add more data based on your specific needs.



		
		cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
		ImGui::End();

		ImGui::Begin("Add Lighting");
		if (ImGui::SliderFloat("Exposure", &sceneExposure, 0.01, 1.0f))
		{
			int sceneExposureLocation = glGetUniformLocation(shaderProgram, "exposure");
			glUniform1f(sceneExposureLocation, sceneExposure);
		}
		ImGui::Text("Add Light");
		ImGui::SliderFloat3("Position", glm::value_ptr(positionNL), -10.0f, 10.0f);
		ImGui::SliderFloat("Strength", &strengthNL, 0.1f, 1.0f);
		ImGui::ColorEdit3("Colour", glm::value_ptr(colourNL));
		if (ImGui::Button("Add Light to Scene"))
		{
			glUseProgram(shaderProgram);
			newLight.position = positionNL;
			newLight.colour = colourNL;
			newLight.strength = strengthNL;
			newLight.pad1 = 0.0f;
			newLight.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
			sceneLights.push_back(newLight);
			int arraySizeLocation = glGetUniformLocation(shaderProgram, "arraySize");
			glUniform1i(arraySizeLocation, sceneLights.size());

			ssboLighting.Bind();
			ssboLighting.updateData(sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size());

					}
		if (ImGui::Button("Remove Last Light"))
		{
			if (sceneLights.size()>0)
				sceneLights.pop_back();
		}
		
		ImGui::End();
		ImGui::Begin("Settings");
		int lightname = 0;
	

		for (auto &source : sceneLights) {
			// Sun position sliders
			
			std::string name = "Sun X " +std::to_string(lightname);
			if (ImGui::SliderFloat(name.c_str(), &source.position.x, -10.0f, 10.0f))
			{
				int arraySizeLocation = glGetUniformLocation(shaderProgram, "arraySize");
				glUniform1i(arraySizeLocation, sceneLights.size());
				std::cout << "\Scene Lights Size(if statement): " << sceneLights.size();
				
				ssboLighting.Bind();
				ssboLighting.updateData(sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size());
			
			}
			name = "Sun Y " + std::to_string(lightname);
			if (ImGui::SliderFloat(name.c_str(), &source.position.y, -10.0f, 10.0f)) {
				int arraySizeLocation = glGetUniformLocation(shaderProgram, "arraySize");
				glUniform1i(arraySizeLocation, sceneLights.size());
				ssboLighting.Bind();
				ssboLighting.updateData(sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size());
			}												
			name = "Sun Z " + std::to_string(lightname);
			if (ImGui::SliderFloat(name.c_str(), &source.position.z, -10.0f, 10.0f)) {
				int arraySizeLocation = glGetUniformLocation(shaderProgram, "arraySize");
				glUniform1i(arraySizeLocation, sceneLights.size());
				ssboLighting.Bind();
				ssboLighting.updateData(sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size());
			}
			name = "Strength " + std::to_string(lightname);
			if (ImGui::SliderFloat(name.c_str(), &source.strength, 0.1f, 1.0f)) {
				int arraySizeLocation = glGetUniformLocation(shaderProgram, "arraySize");
				glUniform1i(arraySizeLocation, sceneLights.size());
				ssboLighting.Bind();
				ssboLighting.updateData(sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size());
			}

			name = "Transparency " + std::to_string(lightname);
			if (ImGui::SliderFloat(name.c_str(), &material.transparency, 0.05f, 1.0f)) {
				ssboMaterial.Bind();
				ssboMaterial.updateData(&material, sizeof(World::Material));
			}

			name = "Diffuse " + std::to_string(lightname);
			if (ImGui::SliderFloat3(name.c_str(), &material.diffuse.x, 0.05f, 1.0f)) {
				ssboMaterial.Bind();
				ssboMaterial.updateData(&material, sizeof(World::Material));
			}

			name = "Ambient " + std::to_string(lightname);
			if (ImGui::SliderFloat3(name.c_str(), &material.ambient.x, 0.05f, 1.0f)) {
				ssboMaterial.Bind();
				ssboMaterial.updateData(&material, sizeof(World::Material));
			}
			// Sun color sliders

			name = "Sun Color " + std::to_string(lightname);
			if(ImGui::ColorEdit3(name.c_str(), glm::value_ptr(source.colour))){
				
				
				ssboLighting.Bind();
			ssboLighting.updateData(sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size());
		}

			lightname++;
		}

	


		int arraySizeLocation = glGetUniformLocation(shaderProgram, "arraySize");
		glUniform1i(arraySizeLocation, sceneLights.size());
	//	glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
	
		ImGui::End();
		// Render pImGui demo window (for testing)
		//ImGui::ShowDemoWindow();
		cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0])* cubesSSBOVector.size());
		dynamicsWorld->updateAabbs();
		dynamicsWorld->stepSimulation(deltaTime, 1);
		//std::cout << "\n\tFrame time: " << deltaTime << "\n\t";
		if (ImGui::Button("Chaos")) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> translationDist(-15, 15);

			btCollisionObjectArray& collisionObjects = dynamicsWorld->getCollisionObjectArray();

			for (int i = 0; i < cubesSSBOVector.size(); i++) {
				// Check if the collision object is a rigid body
				if (collisionObjects[i]->getInternalType() == btCollisionObject::CO_RIGID_BODY) {
					btRigidBody* rigidBody = btRigidBody::upcast(collisionObjects[i]);

					if (rigidBody) {
						btTransform currentTransform;
						rigidBody->getMotionState()->getWorldTransform(currentTransform);

						// Apply transformations or updates as needed
						float randomX = translationDist(gen);
						float randomY = translationDist(gen);
						float randomZ = translationDist(gen);


						// Apply the random translation to the current transform
						btVector3 translation(randomX, randomY, randomZ);
						currentTransform.setOrigin(currentTransform.getOrigin() + translation);

						// Set the updated transform back to the rigid body
						rigidBody->getMotionState()->setWorldTransform(currentTransform);
						rigidBodies[i]->getMotionState()->setWorldTransform(currentTransform);

						// Now you can work with the rigidBody
						// For example, you can access its transform or apply forces
					}
				}

			}

				
		}

		// Update cube positions based on physics simulation
	//	if (cubesSSBOVector.size() > 0)
		{
			if (cubesSSBOVector.size()>0)
			for (int i = 0; i < cubesSSBOVector.size(); i++) {
				btTransform trans;
				cubesSSBOVector[i].rigidBody->getMotionState()->getWorldTransform(trans);

				// Extract translation
				glm::dvec3 newPosition(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());

				// Extract rotation (as a quaternion)
				btQuaternion rotation = trans.getRotation();
				glm::quat newRotation(rotation.w(), rotation.x(), rotation.y(), rotation.z());

				// Extract scale from the collision shape
				btVector3 collisionShapeScale = rigidBodies[i]->getCollisionShape()->getLocalScaling();
				glm::vec3 newScale(collisionShapeScale.x(), collisionShapeScale.y(), collisionShapeScale.z());

				// Create translation matrix
				glm::mat4 translationMatrix = glm::translate(glm::dmat4(1.0f), newPosition);

				// Create rotation matrix
				glm::mat4 rotationMatrix = glm::mat4_cast(newRotation);

				// Create scale matrix
				glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0));

				// Combine translation, rotation, and scale
				cubesSSBOVector[i].modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
			}
			else {
				rigidBodies.clear();
				cubesSSBOVector.clear();
				//cubeSSBO.clearSSBO();

			}
			cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
		}



		
		// Render ImGui UI
		debugger.SetMatrices(camera.getViewMatrix(), projection);
		if (boolRigidBody) { dynamicsWorld->debugDrawWorld(); }// If to draw the physics debug wireframes
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		ssboLighting.Bind();
		mousePickingValue = 1;
		GLint mousePickingUniformLocation = glGetUniformLocation(shaderProgram, "mousePicking");
		glUniform1i(mousePickingUniformLocation, mousePickingValue);
		glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
	//	glViewport(0, 0, window_width, window_height);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glUniform1i(isInstancedBool, 1);
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 5000);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: first draw call (picking) " << error;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUniform1i(isInstancedBool,0);
		cubeTest.draw();
		
	
		
		mousePickingValue = 0;
		mousePickingUniformLocation = glGetUniformLocation(shaderProgram, "mousePicking");
		glUniform1i(mousePickingUniformLocation, mousePickingValue);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glUniform1i(isInstancedBool, 1);
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 5000);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: second draw call" << error;
		}
		//std::cout << "\nMain shader program name: " << shaderProgram;

		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUniform1i(isInstancedBool, 0);
		cubeTest.draw();
		glBindVertexArray(VAO);
		
		
	/*	for (auto item : setDrawLinePositions) {
			std::cout << item << "\n";
		
		}*/
		debugger.SetMatrices(camera.getViewMatrix(), projection);
		if (!terrain.init) terrain.initalise();
		if(terrain.ready)
		terrain.render();
			glUseProgram(shaderProgram);
	
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//debugger.SetMatrices(view, projection);
		//dynamicsWorld->setDebugDrawer(&debugger);
			// 		dynamicsWorld->debugDrawWorld();

		// Swap buffers
		glfwSwapBuffers(window);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: before poll events";
		}
		glfwPollEvents();
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: after poll events";
		}
		//clear colour buffer
	
		//glClearColor(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 1.0f);
			// Clear the color buffer (specify GL_COLOR_BUFFER_BIT)
	

	
		// and any other buffers you want to clear
		
	}
		//cleanUp();

		glfwTerminate();
		ImGui::DestroyContext();

		return 0;
	
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Update the viewport to match the new window size
	GLenum error;
	int framebufferWidth, framebufferHeight;
	glfwGetWindowSize(window, &framebufferWidth, &framebufferHeight);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "Error getting windowsize in framebuffe size callback " << error << std::endl;
	


	}
	window_width = framebufferWidth; // Update your global width
	window_height = framebufferHeight; // Update your global height
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
	glViewport(0, 0, width, height);
	std::cout << "\ncolorFBO buffer ID is: " << colorFBO << "\n";
	// Update any framebuffer attachments that depend on window size
	

	// Update the color texture attachment
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width,height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // Update your global height
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	std::cout << "\n\ncolorTexture  buffer ID is: " << colorTexture << "\n";
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		
	

	}
	// Update the depth renderbuffer attachment
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderBuffer);
	std::cout << "\nDepth render buffer ID is: "<< depthrenderBuffer << "\n";
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at fbo callback at renderbufferstorage depth buffer: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback at renderbufferstorage depth buffer: " << error << std::endl;
	

	}
	// Check framebuffer completeness (for debugging)
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete after resize!" << std::endl;
	}

	// Unbind objects
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at fbo callback fbo callback bind to default: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback fbo callback bind to default: " << error << std::endl;
	


	}
}



//function to clear only partial parts of the view port. This is useful if the window will only be partiually used
//like in a game where the UI may take up other parts of the area.
void viewportClearing()
{

	//viewport clear only
	/*glClearColor(0., 0., 1., 1.);
	glScissor(0, 0, 1060, 600);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);*/

}


void drawUI()

{


	ImGui::Begin("Render Shapes");
	ImGui::Checkbox("Wireframe", &wireframe);

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	const char* options[] = { "Pyramid", "Cube", "Plain", "Sphere" };

	ImGui::Combo("Shape Type", &selectedOption, options, IM_ARRAYSIZE(options));

	//What's needed to create a shape
	
	ImGui::End();

	// Create ImGui window
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
	glViewport(0, 0, window_width, window_height);
	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete! Error code: " << framebufferStatus << std::endl;
		// Handle the error as needed
	}
	
	//create screenshot im gui and tidy up after
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "\nopenGL Error at: Bind framebuffer at gameloop";
	}
	ImGui::Begin("Cubes");
	ImGui::Checkbox("View Physics Engine", &boolRigidBody);
	if (ImGui::Button("Screenshot")) {
		GLubyte* pixels = new GLubyte[window_width * window_height * 4]; // 4 channels (RGBA)
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: BEFORE pixels for saving screenshot\NError creating pixels GLubyte" << error;
		}
		std::cout << "\nScreenshot parameters at time of taking: " << window_width << ", " << window_height << std::endl;
		// Read the pixel data from the framebuffer
		glReadPixels(0, 0, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: read pixels for saving screenshot" << error;
		}
		// Unbind the framebuffer

		// Flip the pixel data vertically (OpenGL reads pixels from bottom to top)
		GLubyte* flippedPixels = new GLubyte[window_width * window_height * 4];

		for (int y = 0; y < window_height; ++y) {
			for (int x = 0; x < window_width; ++x) {
				int sourceIndex = (window_height - y - 1) * window_width + x;
				int destIndex = y * window_width + x;

				memcpy(flippedPixels + destIndex * 4, pixels + sourceIndex * 4, 4);
			}
		}

		// Save the pixel data as an image using stb_image_write
		stbi_write_png("colorFBO.png", window_width, window_height, 4, flippedPixels, window_width * 4);

		// Clean up
		delete[] pixels;
		delete[] flippedPixels;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


		ImGui::Begin("Heightmap Editor");//ImGui::Begin("Heightmap Editor", &showUI);

		// Create a button to generate the heightmap
		  // Iterate through the normals vector and display them in sets of 3
		for (size_t i = 0; i + 2 < terrain.normals.size() && i < 16; i += 3) {
			ImGui::Text("Set %zu", i / 3); // Display a label for the set

			// Display each normal in the set
			for (int j = 0; j < 3; ++j) {
				ImGui::Text("Normal %d: (%.4f, %.4f, %.4f)", j + 1,
					terrain.normals[i + j].x, terrain.normals[i + j].y, terrain.normals[i + j].z);
			}

			ImGui::Separator(); // Add a separator between sets
		}
		ImGui::Checkbox("Draw HeightMap Tools - Slows down rendering for large maps", &boolDrawHeightMap);
	
		if (boolDrawHeightMap) {
			// Button to export the heightmap to a raw file
			if (ImGui::Button("Export to Raw")) {
				std::ofstream file("map.raw", std::ios::binary);
				if (!file.is_open()) {
					std::cerr << "Failed to open file for writing." << std::endl;
					return;
				}

				file.write(reinterpret_cast<const char*>(terrain.heightmapData.heights.data()), terrain.heightmapData.heights.size() * sizeof(float));

				file.close();
				char map[] = "map.raw";
				terrain.loadHeightMap(map, terrain.size);
				terrain.generateHeightMap();
				if (terrain.terrainMeshRigidBody != nullptr) {
					//btCollisionShape* collisionShape = terrain.getTerrainCollionShape();
					dynamicsWorldPtr->removeCollisionObject(terrain.getTerrainMesh());
					terrain.terrainMeshRigidBody = nullptr;
						//delete collisionShape;
				}
				terrain.createTerrainMesh();
			
				
			}
			if (ImGui::SliderFloat("Frequency", &terrain.frequency, 0.01f, 25.0f))
			{
				terrain.generateHeightMap();
			}

			if (ImGui::SliderFloat("Amplitude", &terrain.amplitude, 1.f, 100.0f))
			{
				terrain.generateHeightMap();
			}
			if (ImGui::SliderInt("Size", &terrain.size, 1.f, 1024.0f)) {
				terrain.heightmapData.size = terrain.size;

				terrain.generateHeightMap();
			}


			ImGui::SliderFloat2("Grid", &Grid.x, 2.0f, 30.0f);
			// Button to generate the heightmap
			if (ImGui::Button("Generate Heightmap")) {
				terrain.generateHeightMap();
			}
			if (terrain.size > 0 && terrain.heightmapData.heights.size() > 0) {
				//for (int y = 0; y < terrain.heightmapData.size; ++y) {
				//	ImGui::BeginGroup(); // Begin a row

				//	for (int x = 0; x < terrain.heightmapData.size; ++x) {
				//		int index = x + y * terrain.heightmapData.size;
				//		float heightValue = terrain.heightmapData.heights[index];

				//		// Map height value to grayscale color
				//		ImVec4 color(heightValue, heightValue, heightValue, 1.0f);

				//		// Display color as a square

				//		ImGui::ColorButton("##ColorButton", color, ImGuiColorEditFlags_NoTooltip, ImVec2(Grid.x, Grid.y));

				//		ImGui::SameLine(); // Display the next color in the same row
				//	}

				//	ImGui::EndGroup(); // End the row
				//}

				//ImGui::Text("Heightmap Grid:");
				//for (int y = 0; y < terrain.heightmapData.size; ++y) {
				//	for (int x = 0; x < terrain.heightmapData.size; ++x) {
				//		int index = x + y * terrain.heightmapData.size;
				//		ImGui::Text("%.2f", terrain.heightmapData.heights[index]);
				//		ImGui::SameLine();
				//	}
				//	ImGui::NewLine();
				//}

				//// Create a grid for editing height values
				//for (int y = 0; y < terrain.heightmapData.size; ++y) {
				//	for (int x = 0; x < terrain.heightmapData.size; ++x) {
				//		int index = x + y * terrain.heightmapData.size;
				//		ImGui::InputFloat(("Height##" + std::to_string(index)).c_str(), &terrain.heightmapData.heights[index]);
				//	}
				//}

			}
		}
		ImGui::End();
	


		//Generate new cube IMGUI and their physics body

		if (ImGui::SliderFloat("Ground Position Y", &groundPositionY, -25.5f, 16.5f)) {

			btCollisionObjectArray& collisionObjects = dynamicsWorldPtr->getCollisionObjectArray();

			//digs out the first object in the dynamics world (the 
			btCollisionObject* collisionObject = collisionObjects[0];
			btRigidBody* backupGroundBody = btRigidBody::upcast(collisionObject);


			if (backupGroundBody) {

				btTransform groundTransform = backupGroundBody->getWorldTransform();
				// Rest of your code

				groundTransform = backupGroundBody->getWorldTransform();
				btVector3 currentPosition = groundTransform.getOrigin();
				btScalar currentYBefore = currentPosition.getY();
				std::cout << "Current Y Before Transform: " << currentYBefore << std::endl;

				currentPosition = groundTransform.getOrigin();
				currentPosition.setY(groundPositionY);
				groundTransform.setOrigin(currentPosition);
				groundBody->setWorldTransform(groundTransform);
				//	dynamicsWorld->updateSingleAabb(backupGroundBody);
				btScalar currentYAfter = groundPositionY;
				std::cout << "New Y After Transform: " << currentYAfter << std::endl;
				//	cubeModelMatrix = groundBody->getWorldTransform();



					//Get data for physical representation
				btTransform worldTransform = backupGroundBody->getWorldTransform();
				btVector3 position = worldTransform.getOrigin();
				btQuaternion rotation = worldTransform.getRotation();

				cubeModelMatrix = glm::mat4(1.0f); // Initialize an identity matrix

				// Set the translation component
				cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(position.getX(), position.getY(), position.getZ()));

				// Set the rotation component
				glm::quat glmRotation(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ());
				cubeModelMatrix *= glm::mat4(glmRotation);

			}
			else {

				// Handle the case where backupGroundBody is null
			}


			// Print the current Y before the transform




		}
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeModelMatrix));
		ImGui::InputInt("Cube Number:", &cubesToGenerate);
		// Button to generate cubes



		if (ImGui::Button("Generate Cubes"))
		{
			// Clear the vector
			int loop = 1;
			if (rigidBodies.size() > 0) {
				for (btRigidBody* rigidBody : rigidBodies) {
					//std::cout << "iteration: "<< loop++ << "\n";
					if (rigidBody->getCollisionShape() != nullptr) {
						delete rigidBody->getCollisionShape();
					}
					delete rigidBody->getMotionState();

					dynamicsWorldPtr->removeRigidBody(rigidBody);
					delete rigidBody;
				}
				rigidBodies.clear(); // Clear the rigid bodies vector
			}
			//std::cout << "\nSize of dynamicsWorld after delete iteration:" << dynamicsWorld->getNumCollisionObjects();

			cubesSSBOVector.clear();
			worldObject::nextID = 1;
			/*
				for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i > 0; i--) {
					btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
					btRigidBody* body = btRigidBody::upcast(obj);
					if (body) {
						dynamicsWorld->removeRigidBody(body);
						delete body->getMotionState();
						if (body->getCollisionShape() != NULL)
							delete body->getCollisionShape();
						delete body;
					}

					else {
						dynamicsWorld->removeCollisionObject(obj);
					}
					collisionShapes.clear();
				}
			*///	cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0])* cubesSSBOVector.size());




			btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(500.), btScalar(50.), btScalar(500.)));

			btRigidBody* backupGroundBody = btRigidBody::upcast(groundBody);

			collisionShapes.push_back(backupGroundBody->getCollisionShape());

			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(0, groundPositionY, 0));

			btScalar mass(0.2);

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				groundShape->calculateLocalInertia(mass, localInertia);

			//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);
			//dynamicsWorld->addCollisionObject(body);

			// Generate new cube instances
			std::set<float> set1;
			std::set<float> set2;
			std::mt19937 rng(std::time(nullptr));
			glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

			for (int i = 0; i < cubesToGenerate; i++) {

				btBoxShape* boxShapeInstance = new btBoxShape(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));
				boxShapeInstance->setMargin(0.1f);

				// Define random distribution for positions and velocities
				//std::uniform_real_distribution<float> positionDist(minPos, maxPos); // Define minPos and maxPos as desired
				std::uniform_real_distribution<float> velocityDist(0.1f, 5.0f); // Define minVel and maxVel as desired

				float xVel = velocityDist(rng);
				float yVel = velocityDist(rng);
				float zVel = velocityDist(rng);


				glm::vec3 instancePosition;
				instancePosition.x = glm::linearRand(-1.0f, 147.0f);
				instancePosition.y = glm::linearRand(55.0f, 86.0f);
				instancePosition.z = glm::linearRand(-1.0f, 147.0f);

				//	instancePosition.y = glm::linearRand((1.0f), 34.0f);
				glm::vec3 instanceScale = glm::vec3(0.4f, 0.4f, 0.4f);
				glm::mat4 modelMatrix(1.0f); // Initialize the model matrix as an identity matrix

				// Apply scale
				modelMatrix = glm::scale(modelMatrix, instanceScale);

				// Apply translation
				modelMatrix = glm::translate(modelMatrix, instancePosition);

				// No rotation is applied in this version

				World::cubeInstance cube;
				cube.modelMatrix = modelMatrix;
				//cube pushed back into vector after rigid body creation to pass ptr to body into same ssbo vector so each cube can
					//access its RB

				glm::vec3 cubePosition = instancePosition;
				glm::vec3 scale = instanceScale;



				// Set initial position and orientation
				btTransform startTransform;
				startTransform.setIdentity();
				startTransform.setOrigin(btVector3(cubePosition.x, cubePosition.y, cubePosition.z));

				// Create a rigid body
				btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
				btVector3 localInertia(0, 0, 0);
				boxShapeInstance->calculateLocalInertia(1.0f, localInertia); // 1.0f is the mass

				btRigidBody::btRigidBodyConstructionInfo rbInfo(2.1f, motionState, boxShapeInstance, localInertia);
				btRigidBody* rigidBody = new btRigidBody(rbInfo);

				//rigidBody->setLinearVelocity(btVector3(xVel, yVel, zVel));
				btVector3 angularVelocity(0.0f, 0.0f, 0.0f); // Adjust these values as desired
				rigidBody->setAngularVelocity(angularVelocity);
				// Add rigid body to the dynamics world
				dynamicsWorldPtr->addRigidBody(rigidBody);
				cube.rigidBody = rigidBody;
				rigidBodies.push_back(rigidBody);
				cubesSSBOVector.push_back(cube);
				collisionShapes.push_back(boxShapeInstance);

			}
			// add the body to the dynamics world


			// Update the SSBO with the new data
			cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());

		}

		//if (ImGui::Button("Display Physics Data"))
		ImGui::Begin("Physics Data");

		for (int i = 0; i < dynamicsWorldPtr->getNumCollisionObjects(); i++) {
			btCollisionObject* obj = dynamicsWorldPtr->getCollisionObjectArray()[i];

			// Check if it's a rigid body
			btRigidBody* rigidBody = btRigidBody::upcast(obj);
			if (rigidBody) {
				// Print relevant data about the rigid body
				btCollisionShape* shape = rigidBody->getCollisionShape();
				btVector3 position = rigidBody->getCenterOfMassPosition();
				btVector3 velocity = rigidBody->getLinearVelocity();
				btVector3 angularVelocity = rigidBody->getAngularVelocity();

				// Get the size of the collision shape (assuming it's a box shape)
				btVector3 halfExtents;

				if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE) {
					btBoxShape* boxShape = static_cast<btBoxShape*>(shape);
					halfExtents = boxShape->getHalfExtentsWithMargin();
				}

				else if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
					ImGui::Text("Rigid Body Sphere");
				}
				else {
					// Handle other shape types here if needed
					ImGui::Text("Unknown Collision Shape Type");

					continue; // Skip to the next collision object
				}

				btVector3 size = halfExtents * 2.0;

				// Display data in ImGui window
				ImGui::Text("Rigid Body %d", i);
				if (ImGui::Button(("Look at Shape " + std::to_string(i)).c_str())) {
					// When the button is clicked, set the camera's view matrix to look at the shape
					// You'll need to adjust this part based on your camera setup
					//glm::vec3 cameraPosition = /* Set your camera position here */;
					camera.mPitch = 0.0f;  // Reset pitch to 0 degrees
					camera.mYaw = -90.0f;
					camera.update();
					view = glm::lookAt(camera.mPosition, glm::vec3(position.x(), position.y(), (position.z() + 3.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
					camera.mPitch = 0.0f;  // Reset pitch to 0 degrees
					camera.mYaw = -90.0f;
					glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

					glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
					camera.mViewMatrix = view;
					camera.mPosition.x = position.x();
					camera.mPosition.y = position.y();
					camera.mPosition.z = (position.z() + 3.0f);
				}
				glm::mat4 viewMatrix = glm::lookAt(camera.mPosition, glm::vec3(position.x(), position.y(), position.z() + 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

				for (int row = 0; row < 4; ++row) {
					ImGui::Text("%.3f, %.3f, %.3f, %.3f", viewMatrix[row][0], viewMatrix[row][1], viewMatrix[row][2], viewMatrix[row][3]);
				}
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
				//	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

				ImGui::Text("Position: (%f, %f, %f)", (std::numeric_limits<double>::max_digits10, position.x()),
					(std::numeric_limits<double>::max_digits10, position.y()),
					(std::numeric_limits<double>::max_digits10, position.z()));

				ImGui::Text("Size: (%f, %f, %f)", size.x(), size.y(), size.z());
				ImGui::Text("Linear Velocity: (%f, %f, %f)", velocity.x(), velocity.y(), velocity.z());
				ImGui::Text("Angular Velocity: (%f, %f, %f)", angularVelocity.x(), angularVelocity.y(), angularVelocity.z());
				ImGui::Text(" ");
			}
			else {
				// It's not a rigid body, so it might be another type of collision object
				// You can add handling for other types here if needed
				ImGui::Text("Unknown Collision Object Type");
				ImGui::Text("Other Body %d", i);
				btCollisionShape* shape = obj->getCollisionShape();
				btTransform worldtransform = obj->getWorldTransform();
				btVector3 position = worldtransform.getOrigin();
				btVector3 velocity = obj->getInterpolationLinearVelocity();
				//btVector3 angularVelocity = obj->getAngularVelocity();
				btSphereShape* sphereShape = static_cast<btSphereShape*>(shape);
				int size;
				//size = sphereShape->getRadius();
				glm::mat4 viewMatrix = glm::lookAt(camera.mPosition, glm::vec3(position.x(), position.y(), position.z() + 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

				if (ImGui::Button(("Look at Shape " + std::to_string(i)).c_str())) {




					// When the button is clicked, set the camera's view matrix to look at the shape
					// You'll need to adjust this part based on your camera setup
					//glm::vec3 cameraPosition = /* Set your camera position here */;
					camera.mPitch = 0.0f;  // Reset pitch to 0 degrees
					camera.mYaw = -90.0f;
					camera.update();
					view = glm::lookAt(camera.mPosition, glm::vec3(position.x(), position.y(), (position.z() + 3.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
					camera.mPitch = 0.0f;  // Reset pitch to 0 degrees
					camera.mYaw = -90.0f;
					glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

					glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
					camera.mViewMatrix = view;
					camera.mPosition.x = position.x();
					camera.mPosition.y = position.y();
					camera.mPosition.z = (position.z() + 3.0f);
				}
				for (int row = 0; row < 4; ++row) {
					ImGui::Text("%.3f, %.3f, %.3f, %.3f", viewMatrix[row][0], viewMatrix[row][1], viewMatrix[row][2], viewMatrix[row][3]);
				}
				ImGui::Text("Position: (%f, %f, %f)", position.x(), position.y(), position.z());
				//ImGui::Text("Radius: (%f)", size);
				ImGui::Text("Linear Velocity: (%f, %f, %f)", velocity.x(), velocity.y(), velocity.z());
				//ImGui::Text("Angular Velocity: (%f, %f, %f)", angularVelocity.x(), angularVelocity.y(), angularVelocity.z());
				ImGui::Text(" ");
			}
		}
		//<< section hidden via tool bar on left near line numberr <<
		ImGui::End();


}






void addCubes(GLuint shader, Mesh mesh) {

	ImGui::Begin("Create Cubes");
	if (ImGui::Button("Add Cube")) {
	
		
		
	}

	if (ImGui::Button("Delete Cube") && instVector.size()>0 ) {
		
	
	}


	

	ImGui::End();

}

//void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
//	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
//
//
//	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ) {
//		
//
//		lMouseClicked = true;
//		
//		double xpos, ypos;
//		int width, height;
//		
//		glfwGetFramebufferSize(window, &width, &height);
//		glfwGetCursorPos(window, &xpos, &ypos);
//		//std::cout << "\nglfw sees x: " << xpos << ", y: " << ypos << std::endl;
//		int glX = static_cast<int>(xpos);
//		int glY = height - static_cast<int>(ypos) - 1;
//
//		GLubyte pixelColor[4];
//				
//		GLint currentFramebuffer;
//		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);
//		GLenum error;
//		if (currentFramebuffer == colorFBO) {
//			// The colorFBO is currently bound as the framebuffer. You can safely call glReadPixels here
//			
//			glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
//			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
//			while ((error = glGetError()) != GL_NO_ERROR) {
//				std::cout << "OpenGL Error: " << error << std::endl;
//
//
//
//			}
//			//std::cout << "\nMouse co ordinates, X:" << glX << " Y:" << glY << std::endl;
//			std::cout << "\n" << static_cast<int>(pixelColor[0]) << "," << static_cast<int>(pixelColor[1]) << ", " << static_cast<int>(pixelColor[2]) << ", " << static_cast<int>(pixelColor[3]);
//			rgbSelected[0] = pixelColor[0];
//			rgbSelected[1] = pixelColor[1];
//			rgbSelected[2] = pixelColor[2];
//			rgbSelected[3] = pixelColor[3];
//			std::cout << "\nRGB seleted: " <<rgbSelected[0] << ", " << rgbSelected[1] << ", " << rgbSelected[2];
//			glm::vec3 colour;
//			colour.r = rgbSelected[0];
//
//			colour.g = rgbSelected[1];
//			colour.b = rgbSelected[2];
//			int deleteVector = -1;
//			deleteVector = (findCubeIndexByColour(glm::vec3(pixelColor[0], pixelColor[1], pixelColor[2])));
//				if (deleteVector != -1 && cubesSSBOVector.size()>0) {
//				
//				//dynamicsWorldPtr->removeRigidBody(
//				std::cout << "\nVector index to delete: " << deleteVector;
//				std::cout << "\ncube ID: " << cubesSSBOVector[deleteVector].ID;
//				dynamicsWorldPtr->removeRigidBody(cubesSSBOVector[deleteVector].rigidBody);
//				cubesSSBOVector.erase(cubesSSBOVector.begin() + (deleteVector));
//				cubeSSBOptr->updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
//
//				//for (int i = dynamicsWorldPtr->getNumCollisionObjects() - 1; i >= 0; i--) {
//				//	btCollisionObject* obj = dynamicsWorldPtr->getCollisionObjectArray()[i];
//				//	btRigidBody* rigidBody = btRigidBody::upcast(obj);
//
//				//	if (rigidBody && rigidBody->getUserPointer() == reinterpret_cast<void*>(deleteVector)) {
//				//		std::cout << "\nUser Pointer value of click cube: " << (int)rigidBody->getUserPointer();
//				//		// Found the rigid body with the matching user data (ID)
//				//		
//
//
//				//		std::cout << "\nRigid Body Vector Size: "<<rigidBodyVectorPtr->size();
//				//		
//				//		std::cout << "\nRigid Body Vector Size: " << rigidBodyVectorPtr->size();
//
//				//		std::cout << "\nNumber of collision objects" << dynamicsWorldPtr->getNumCollisionObjects();
//				//		
//				//		dynamicsWorldPtr->removeRigidBody(rigidBody);
//				//		
//				//		delete rigidBody->getMotionState();
//				//		delete rigidBody->getCollisionShape();
//				//		
//				//		delete rigidBody;
//				//		std::cout << "\nNumber of collision objects:" << dynamicsWorldPtr->getNumCollisionObjects();
//
//				//		break; // Assuming each ID is unique, you can break out of the loop once you find a match.
//				//	}
//				//	rigidBodyVectorPtr->erase(rigidBodyVectorPtr->begin() + deleteVector);
//				//}
//				
//
//				
//
//			}
//		}
//		else {
//			std::cout << "\nColor buffer not bound";
//			glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
//			while ((error = glGetError()) != GL_NO_ERROR) {
//				//	std::cout << "OpenGL Error: " << error << std::endl;
//				glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
//				//	std::cout << "\nMouse co ordinates, X:" << glX << " Y:" << glY << std::endl;
//			}
//		}
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		//std::cout << static_cast<int>(pixelColor[0]) << "," << static_cast<int>(pixelColor[1]) << ", " << static_cast<int>(pixelColor[2]) << ", " << static_cast<int>(pixelColor[3]);
//		//rgbSelected[0] = pixelColor[0];
//		//rgbSelected[1] = pixelColor[1];
//		//rgbSelected[2] = pixelColor[2];
//		//rgbSelected[3] = pixelColor[3];
//		//glm::vec3 colour;
//		//colour.r = rgbSelected[0];
//
//		//colour.g = rgbSelected[1];
//		//colour.b = rgbSelected[2];
//		//int deleteVector = (findCubeIndexByColour(glm::vec3(pixelColor[0], pixelColor[1], pixelColor[2])));
//		//if (deleteVector != -1) {
//		//	std::cout << "vector idnex to delete: " << deleteVector;
//		//	cubesSSBOVector.erase(cubesSSBOVector.begin() + deleteVector);
//		//	cubeSSBOptr->updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
//		//	deleteVector = 0;
//		//	
//		//	}
//
//
//
//	}
//	else {
//		//Left Mouse Button is not clicked
//		lMouseClicked = false;
//
//	}
//
//
//}

Cube* findCubeByColour(const glm::vec3 color) {

	//for (Cube& cube : drawmesVector) {
	//	if (cube.objectColour == color) {
	//	
	//		return &cube;
	//	}


	//}
	return nullptr;
}

int findCubeIndexByColour(const glm::vec3 color) {

	if (cubesSSBOVector.size() > 0) {
		//std::cout << "\nInside find cube function- colour passed to function is:" << color.r << ", " << color.g << ", " << color.b << std::endl;
		//std::cout << "Example cube at location 0 color * 255 is:" << cubesSSBOVector[0].objectColour.r * 255 << ", " << cubesSSBOVector[0].objectColour.g * 255 << ", " << cubesSSBOVector[0].objectColour.b * 255;
		//check the picked colour against colour and return the vector index to calling function. (to delete it)
		for (auto it = cubesSSBOVector.begin(); it != cubesSSBOVector.end(); ++it) {
			//std::cout << "\nVector colours: " << it->objectColour.r * 255 << ", " << it->objectColour.g * 255 << ", " << it->objectColour.b * 255;

			if (static_cast<int>(it->objectColour.r * 255) == color.r &&
				static_cast<int>(it->objectColour.g * 255) == color.g &&
				static_cast<int>(it->objectColour.b * 255) == color.b) {
				// Remove the element that meets the condition

				dynamicsWorldPtr->removeRigidBody(it->rigidBody);
				it = cubesSSBOVector.erase(it);
				cubeSSBOptr->updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());

				// The iterator 'it' now points to the next element after the erased one.
				// If you want to continue checking the current position, decrement 'it'.
				// --it;
			//	std::cout << "\nCubeSSBOVector size is: " << cubesSSBOVector.size();
				return -1;

				//std::cout << "\nAbsolute i to delete is: " << std::distance(cubesSSBOVector.begin(), it);
			}
		}
	}
	
	return -1;
}

glm::mat4 createCamera(glm::vec3& cameraPosition, glm::vec3& targetPosition, glm::vec3& upVector)

{
	return glm::lookAt(cameraPosition, targetPosition, upVector);
}

void cleanUp() {
	
};


void instanceCubeFunction(GLuint VAO, GLuint VBO, GLuint EBO) {

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//nullptr for below in third parameter ?
	glBufferData(GL_ARRAY_BUFFER, instCubeVertices.size() * sizeof(float), nullptr, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, instCubeIndices.size() * sizeof(unsigned int), instCubeIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0)); // Position attribute
	
	GLuint totalVertices = instVector.size() * 8;//(how many cubes * the amount of vertices per cute)

	size_t offset = 0;

	for (const auto& cube : instVector) {
		//	 Copy vertex data for the current cube to the VBO
			//glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(cubeVertices) * 8, cube.vertices.data());
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float) * cube.vertices.size(), cube.vertices.data());
		// Increment the offset for the next cube



		// Increment the offset for the next cube
		offset += sizeof(float) * cube.vertices.size();

	}
	
	//glBindVertexArray(0);

}


std::string GetShapeTypeString(int shapeType) {
	switch (shapeType) {
	case BOX_SHAPE_PROXYTYPE:
		return "Box Shape";
	case SPHERE_SHAPE_PROXYTYPE:
		return "Sphere Shape";
	case CYLINDER_SHAPE_PROXYTYPE:
		return "Cylinder Shape";
	case CAPSULE_SHAPE_PROXYTYPE:
		return "Capsule Shape";
	case CONE_SHAPE_PROXYTYPE:
		return "Cone Shape";
		// Add cases for other shape types as needed
	default:
		return "Unknown Shape Type";
	}
}

// Example usage:


void initialise(float x, float y, float z, GLFWwindow* window) {
	//xyz are for character parameters
	character = new Character(dynamicsWorldPtr, x, y, z, window );
	camera.m_window = window;


	//setup matrices

	 modelLoc = glGetUniformLocation(*defaultShaderProgramPtr, "model");
	 viewLoc = glGetUniformLocation(*defaultShaderProgramPtr, "view");
	 projectionLoc = glGetUniformLocation(*defaultShaderProgramPtr, "projection");


	 glm::mat4  model = glm::mat4(1.0f);
	 glm::mat4 view = glm::lookAt(glm::vec3(0.5f, -0.7f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	 glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 30000.0f);

	  cubeModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
	
}

void update() {
	character->update();
}


