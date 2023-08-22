#define _CRT_SECURE_NO_WARNINGS 
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"
#include <stb/stb_image.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "globals.h"

#include <iostream>

#include "btBulletDynamicsCommon.h"
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
struct Mesh {
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};
std::vector<Cube> instVector;

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

std::vector<World::cubeInstance> cubesSSBOVector;
 GLuint colorFBO;
GLuint colorTexture;
GLuint depthrenderBuffer;
 int rgbSelected[3];

void drawUI();
void instanceCubeFunction(GLuint VAO, GLuint VBO, GLuint EBO);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void addCubes(GLuint shader, Mesh mesh);
Cube* findCubeByColour(const glm::vec3 color); //used to access teh cubes properties or ptr to cube
int findCubeIndexByColour(const glm::vec3 color); //used to delete items from vector
SSBO* cubeSSBOptr= nullptr;
glm::mat4 createCamera(glm::vec3& cameraPosition, glm::vec3& targetPosition, glm::vec3& upVector);


//initialise debug openGL 4.3+ callback
void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam) {
	// Handle the debug message based on the severity, type, etc.
	// For example, you can print the message to the console.
	//std::cout << "\t " << source << "Severity: " << severity << std::endl;
	
}

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

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	///-----initialization_end-----

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	///create a few basic rigid bodies

	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -56, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}

	{
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
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

		startTransform.setOrigin(btVector3(2, 10, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}



	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(1200, 1200, "Shape Engine", NULL, NULL);


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

	glViewport(0, 0, window_width, window_height);

	// resizes viewport based on automatic resize, passes parameters to the gl viewport


	//registers to GLFW the function,


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = static_cast<float>(window_width);
	io.DisplaySize.y = static_cast<float>(window_height);

	//ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	//Create buffers.

	
	//specify layout for the 



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
	unsigned char* imgData = stbi_load("kiss.jpg", &imgWidth, &imgHeight, &colChannels, 3);
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
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
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


	//normalsVertices = DoMath::generateNormalVector(cubeVerticesPure, cubeIndices);
	
	//print out normals.
	/*for (auto floats : cubeNormals)
	{
		std::cout << "\n" << floats.x << ", " << floats.y << ", " << floats.z << "\n";
	}*/

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* cubeIndices.size(), cubeIndices.data(), GL_STATIC_DRAW);
	
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, (sizeof(float)*cubeVertices.size()) + (3* sizeof(float) * cubeNormals.size()), NULL, GL_STATIC_DRAW);
		// Upload vertex data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)* cubeVertices.size(), cubeVertices.data());
		// Upload normals data
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)* cubeVertices.size(), 3*sizeof(float)* cubeNormals.size(), cubeNormals.data());

	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)* cubeVertices.size(), sizeof(glm::vec3)* normalsVertices.size(), normalsVertices.data());
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after setting up VAO attributes: " << error << std::endl;



	}


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)* cubeVertices.size()));
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

	GLuint colorTexture; //Global variables, see header

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


	Camera camera(window);
	glUseProgram(shaderProgram);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error afterpolygon mode " << error << std::endl;

	}
	glClearColor(0.1, 0.1, 0.2, 0.0);
	glEnable(GL_DEPTH_TEST);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after enabling depth test: " << error << std::endl;

	}
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.5f, -0.7f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);

	
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	int viewLoc = glGetUniformLocation(shaderProgram, "view");
	int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	int textureUniformLoc = glGetUniformLocation(shaderProgram, "texture1");
	glUniform1i(textureUniformLoc, 0); // 0 corresponds to GL_TEXTURE0
	
	float rotation = 0.15;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after enabling glblend: " << error << std::endl;

	}

	std::vector<World::Lights> sceneLights;
	//Generate Lights
	World::Lights sun;
	World::Lights lamp1;
	World::Lights stars;
	sun.position = glm::vec3(2.0, 2.0, 1.0); // Example position, adjust as needed
	sun.colour = glm::vec3(1.9, 0.1,0.1);   // Example color, adjust as needed
	sun.strength = 0.2;                 // Example strength, adjust as needed
	sun.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
	lamp1.position = glm::vec3(1.0, -3.0, 2.0); // Example position, adjust as needed
	lamp1.colour = glm::vec3(1.0, 0.1, 0.0);   // Example color, adjust as needed
	lamp1.strength =0.2;                 // Example strength, adjust as needed
	lamp1.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
	stars.position = glm::vec3(1.0, -3.0, 2.0); // Example position, adjust as needed
	stars.colour = glm::vec3(1.0, 1.0, 0.0);   // Example color, adjust as needed
	stars.strength = 0.2;
	stars.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
	World::Material material;
	material.ambient = glm::vec3(0.2, 0.2, 0.2); // Example ambient color, adjust as needed
	material.diffuse = glm::vec3(0.5, 0.5, 0.5); // Example diffuse color, adjust as needed
	material.specular = glm::vec3(0.4, 0.4, 0.4); // Example specular color, adjust as needed
	material.shininess =1.0;             // Example shininess, adjust as needed
	material.transparency = 1;
	// Get the location of the Material uniform in the shader program

	sceneLights.push_back(sun);
	sceneLights.push_back(stars);
	sceneLights.push_back(lamp1);
	sceneLights.push_back(sun);
	//sceneLights.push_back(stars);
	// Upload the sceneLights data to the shader
	
	SSBO ssboLighting(0, sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size(), GL_DYNAMIC_DRAW);
	SSBO ssboMaterial(1, &material, sizeof(World::Material), GL_DYNAMIC_DRAW);
	
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
	
	
	for (int i = 0; i < 2000; i++) {
		World::cubeInstance cube;
		glm::vec3 instancePosition = glm::linearRand(glm::vec3(-100.0f), glm::vec3(100.0f));
		glm::vec3 instanceRotation = glm::linearRand(glm::vec3(-180.0f), glm::vec3(180.0f));
		glm::vec3 instanceScale = glm::linearRand(glm::vec3(0.05f), glm::vec3(9.0f));
		glm::mat4 modelMatrix(1.0f);
		glm::vec3 rotation = glm::linearRand(glm::vec3(-180.0f), glm::vec3(180.0f));

	
		modelMatrix = glm::translate(modelMatrix, instancePosition);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, instanceScale);

		cube.modelMatrix = modelMatrix;
		cube.rotationPerFrame = rotation;
	
		cubesSSBOVector.push_back(cube);
	}
	int numCubes = 0;
	SSBO cubeSSBO(2, cubesSSBOVector.data(), sizeof(cubesSSBOVector[0])* cubesSSBOVector.size(), GL_DYNAMIC_DRAW);
	cubeSSBOptr = &cubeSSBO;
	while (!glfwWindowShouldClose(window))
	{

		//bullet

		///-----stepsimulation_start-----
		for (i = 0; i < 150; i++)
		{
			dynamicsWorld->stepSimulation(1.f / 60.f, 10);

			//print positions of all objects
			for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
			{
				btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
				btRigidBody* body = btRigidBody::upcast(obj);
				btTransform trans;
				if (body && body->getMotionState())
				{
					body->getMotionState()->getWorldTransform(trans);
				}
				else
				{
					trans = obj->getWorldTransform();
				}
				printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			}
		}


		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(debugCallback, nullptr);

		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at game loop start: " << error << std::endl;



		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window_width, window_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
		GLuint viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos");

		// Set the viewPos uniform to the camera's position
		glm::vec3 cameraPosition = camera.mPosition; // Change this to your camera's position
		glUniform3fv(viewPosLocation, 1, glm::value_ptr(cameraPosition));

		float time = glfwGetTime();
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

		float currentTime = glfwGetTime(); // Get current time in seconds
		
		float angle = rotationSpeed * time;

		float cameraX = center.x + radius * cos(angle);
		float cameraZ = center.z + radius * sin(angle);

		camera.mPosition = glm::vec3(cameraX, camera.mPosition.y, cameraZ);
		camera.setViewMatrix();
			ssboLighting.updateData(sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size());

				glUseProgram(shaderProgram);
		camera.update();
		
	
		// Set up uniforms
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0, 1.0, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
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

		// Create ImGui window
		glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: Bind framebuffer at gameloop";
		}
		ImGui::Begin("Cubes");
		if (ImGui::Button("Screenshot")) {
			GLubyte* pixels = new GLubyte[window_width * window_height* 4]; // 4 channels (RGBA)

			// Read the pixel data from the framebuffer
			glReadPixels(0, 0, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "\nopenGL Error at: read pixels for saving screenshot";
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
		glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
		glViewport(0, 0, window_width, window_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Input box for the number of cubes
		ImGui::InputInt("Number of Cubes", &numCubes);

		// Button to generate cubes
		if (ImGui::Button("Generate Cubes"))
		{
			// Clear the vector
			cubesSSBOVector.clear();

			// Generate new cube instances
			for (int i = 0; i < numCubes; i++) {
				glm::vec3 instancePosition = glm::linearRand(glm::vec3(-33.0f), glm::vec3(33.0f));
				glm::vec3 instanceRotation = glm::linearRand(glm::vec3(-180.0f), glm::vec3(180.0f));
				glm::vec3 instanceScale = glm::linearRand(glm::vec3(0.5f), glm::vec3(1.3f));
				glm::mat4 modelMatrix(1.0f);
				modelMatrix = glm::translate(modelMatrix, instancePosition);
				modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMatrix = glm::scale(modelMatrix, instanceScale);
				World::cubeInstance cube;
				cube.modelMatrix = modelMatrix;
				cubesSSBOVector.push_back(cube);
			}

			// Update the SSBO with the new data
			cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
		}

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
		// Render ImGui demo window (for testing)
		ImGui::ShowDemoWindow();

		// Render ImGui UI
		
		glBindVertexArray(VAO);
		ssboLighting.Bind();
		mousePickingValue = 1;
		GLint mousePickingUniformLocation = glGetUniformLocation(shaderProgram, "mousePicking");
		glUniform1i(mousePickingUniformLocation, mousePickingValue);
		glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
	//	glViewport(0, 0, window_width, window_height);


		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 50000);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: first draw call (picking) ";
		}
		mousePickingValue = 0;
		mousePickingUniformLocation = glGetUniformLocation(shaderProgram, "mousePicking");
		glUniform1i(mousePickingUniformLocation, mousePickingValue);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 50000);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: second draw call";
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
	window_width = width;
	window_height = height;
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
	glViewport(0, 0, width, height);

	// Update any framebuffer attachments that depend on window size
	

	// Update the color texture attachment
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
		
	

	}
	// Update the depth renderbuffer attachment
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
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
	glClearColor(0., 0., 1., 1.);
	glScissor(0, 0, 1060, 600);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

}


void drawUI()

{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

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
}


void addCubes(GLuint shader, Mesh mesh) {

	ImGui::Begin("Create Cubes");
	if (ImGui::Button("Add Cube")) {
	
		
		
	}

	if (ImGui::Button("Delete Cube") && instVector.size()>0 ) {
		
	
	}


	

	ImGui::End();

}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);


	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		

		lMouseClicked = true;
		
		double xpos, ypos;
		int width, height;
		
		glfwGetFramebufferSize(window, &width, &height);
		glfwGetCursorPos(window, &xpos, &ypos);
		//std::cout << "\nglfw sees x: " << xpos << ", y: " << ypos << std::endl;
		int glX = static_cast<int>(xpos);
		int glY = height - static_cast<int>(ypos) - 1;

		GLubyte pixelColor[4];
				
		GLint currentFramebuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);
		GLenum error;
		if (currentFramebuffer == colorFBO) {
			// The colorFBO is currently bound as the framebuffer. You can safely call glReadPixels here
			glTextureBarrier();
			glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;



			}
			//std::cout << "\nMouse co ordinates, X:" << glX << " Y:" << glY << std::endl;
			std::cout << "\n" << static_cast<int>(pixelColor[0]) << "," << static_cast<int>(pixelColor[1]) << ", " << static_cast<int>(pixelColor[2]) << ", " << static_cast<int>(pixelColor[3]);
			rgbSelected[0] = pixelColor[0];
			rgbSelected[1] = pixelColor[1];
			rgbSelected[2] = pixelColor[2];
			rgbSelected[3] = pixelColor[3];
			glm::vec3 colour;
			colour.r = rgbSelected[0];

			colour.g = rgbSelected[1];
			colour.b = rgbSelected[2];
			int deleteVector = (findCubeIndexByColour(glm::vec3(pixelColor[0], pixelColor[1], pixelColor[2])));
			if (deleteVector != -1) {
				cubesSSBOVector.erase(cubesSSBOVector.begin() + deleteVector);
				cubeSSBOptr->updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
			
			}
		}
		else {
			std::cout << "\nColor buffer not bound";
			glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
			while ((error = glGetError()) != GL_NO_ERROR) {
				//	std::cout << "OpenGL Error: " << error << std::endl;
				glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
				//	std::cout << "\nMouse co ordinates, X:" << glX << " Y:" << glY << std::endl;
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//std::cout << static_cast<int>(pixelColor[0]) << "," << static_cast<int>(pixelColor[1]) << ", " << static_cast<int>(pixelColor[2]) << ", " << static_cast<int>(pixelColor[3]);
		rgbSelected[0] = pixelColor[0];
		rgbSelected[1] = pixelColor[1];
		rgbSelected[2] = pixelColor[2];
		rgbSelected[3] = pixelColor[3];
		glm::vec3 colour;
		colour.r = rgbSelected[0];

		colour.g = rgbSelected[1];
		colour.b = rgbSelected[2];
		int deleteVector = (findCubeIndexByColour(glm::vec3(pixelColor[0], pixelColor[1], pixelColor[2])));
		if (deleteVector != -1) {
		

			
			}



	}
	else {
		//Left Mouse Button is not clicked
		lMouseClicked = false;

	}


}

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


	//check the picked colour against colour and return the vector index to calling function. (to delete it)
	for (int i = 0; i < cubesSSBOVector.size(); ++i) {
		if (static_cast<int>(cubesSSBOVector[i].objectColour.r*255) == color.r && static_cast<int>(cubesSSBOVector[i].objectColour.g*255) == color.g && static_cast<int>((cubesSSBOVector[i].objectColour.b*255)) == color.b) {
			// print out the contensts of both colour types to make sure they're matching
			return i;
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
