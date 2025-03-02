#define _CRT_SECURE_NO_WARNINGS 
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define BT_USE_DOUBLE_PRECISION
#define _USE_MATH_DEFINES
#include <cmath>
#include <winrt/Windows.Foundation.h>
#include <exception>  // Standard header for exceptions like std::exception

#include <windows.h>
#include <commdlg.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "globals.h"
#include "CallbackFunctions.h"
#include <random>
#include <iostream>
#include <istream>//Do I need both?
#include "ObjImporter.h"//should be this included after assimp below?
#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ImGuiVariables.h"
#include <glm/gtx/compatibility.hpp> // for glm::lerp

#include <cstdlib> // for rand
//Bullet libraries for PHYSICS
#include "C:\Users\ryanb\Desktop\openGL-Engine-0.1\openGL Engine 0.1\libraries\include\bullet\BulletDynamics\btBulletDynamicsCommon.h"
#include "CubeMap.h"
#include "C:\Users\ryanb\Desktop\openGL-Engine-0.1\openGL Engine 0.1\libraries\include\bullet\btBulletCollisionCommon.h"

#include "C:\Users\ryanb\Desktop\openGL-Engine-0.1\openGL Engine 0.1\libraries\include\bullet\LinearMath/btAlignedObjectArray.h"
#include "C:\Users\ryanb\Desktop\openGL-Engine-0.1\openGL Engine 0.1\libraries\include\bullet\BulletDynamics\Dynamics\btRigidBody.h"


#include "C:\Users\ryanb\Desktop\openGL-Engine-0.1\openGL Engine 0.1\libraries\include\bullet\BulletCollision\CollisionDispatch\btCollisionWorld.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <map>
#include "UIManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "worldObject.h"
#include "cubeData.h"

#include "Cube.h"
#include "Lights.h"
#include "SSBO.h"
#include "openGLDrawable.h"
#include "PhysDebug.h"
#include "Engine.h" //Main TERRAIN class
#include "Character.h"
#include <filesystem>
#include "Mesh.h"
#include "Texture.h"
#include "OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh"
#include "waterRenderer.h"
#include "waterShader.h"
#include "waterTile.h"
#include "waterFrameBuffer.h"
#include "tinyfiledialogs.h"
#include "Grid.h"
#include "Animation.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "text_fonts_glyphs.h"
#include "Gizmo.h"
bool boolShowGrid = true;//Engine renders 3d axis grid
bool boolShowWater = true;//Engine ui renders water
//char const* lTheOpenFileName;
#include "ImGuiLogger.h"
#include "ModelLoader.h"
typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;
UIManager* uimanager;
ObjImporter* objimporter;
float deltaTimeGlobal = 0.0;
ImGuiLogger logger;
modelLoader modelLoaderNew;

float currentTime, deltaTime;
//Customer shader variables
std::string customVertexShaderCode;
std::string customFragmentShaderCode;
std::vector<std::string> shaderIDStrings; //strings for shader IDs in drop down for changing them in mesh propertie imgui / 
 int selectedShaderID = 0; //initalisation for shader ID selected where loading meshes
 int selectedShaderID2 = 0;
 struct World::lerpContainer;
 World::lerpContainer lightSceneLerp;
 WorldGrid* worldgridPtr = nullptr;
 std::map<std::string, FramebufferObject> framebuffers;
 AnimationController* animationControllerPtr = new AnimationController();//pointer to access animation controller globally
 std::vector<ObjImporter> models;
 float scalemin, scalemax;
  int meshCountInstanced;
  std::vector<SSBO*> SSBOVector; //initialising external global for SSBO vector for instanced ssbos / grass etc
  std::map<SSBO, std::vector<World::cubeInstance>> mapSSBOMeshInstanceVector;
  Tool currentTool = Tool::NONE;
  GLuint terrainPickingSwitch = 0;
  bool boolToolResized = false;
   bool loadModelTwo = false;
   modelNew* activeModelPtr = nullptr;
  int brushSize;
  std::vector <WaterTile> waterTileVector;
  std::set<std::pair<float, float>> waterBounds;//locations for not creating land objects
  std::vector<customShader*> customShaders; //declaring from globals.h once for all 
std::vector<World::meshInstance*> meshInstancesVector;
//customer shader manager varaibles ends

Mesh mesh;
 ImVec4 bgColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);     // Default background color
 ImVec4 buttonColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Default button color
 bool boolDrawUI=true; //globals.h (left alt  - hot path was in UI! drawimgui)#
 bool drawIMGUI = true;
 bool boolShowGLErrors = false ;
 bool UIdrawn = false;
// Declare and define the global variables
glm::mat4 cubeModelMatrix;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::lookAt(glm::vec3(0.5f, -0.7f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 30000.0f);

GLuint* defaultShaderProgramPtr = new GLuint;
GLuint* globalWaterShader = new GLuint;


GLuint captureTexture = 0;//used for rendering world to imgui window 
//This means the world can be rendered into an additional / separate UI box ...

btRigidBody* groundBody;
int modelLoc;
int viewLoc;
int projectionLoc;
SSBO cubeSSBO;
std::vector<btRigidBody*> rigidBodies;
btAlignedObjectArray<btCollisionShape*> collisionShapes;
bool lMouseClicked = true;
bool terrainLMouseClicked = false; // needed to be declared outside of scope
Camera camera;
float globalTime;
GLuint modelMatrixLocation = 0;
Character* character = nullptr;
 float characterCameraXOffset = 0.0;
 float characterCameraYOffset = 5.0;
 float characterCameraZOffset = 0.0;

//forward declarations
 void showLoadModelWindow(bool* pOpen); // function loads models as a secon way
 void debugPrintPixels(const unsigned char* pixels, int width, int height);//function for checking pixel array for screen buffer values 
 void CC(std::string location, std::string comment = "debug");
 void generateSSBOInstance(int count, std::string ssboname); //function used to create instance date for dynamic mesh ssbos
 void generateSSBOInstanceToY(const std::vector<float>& terrainHeights, int terrainSize, int numberOfMeshes, float minY, float maxY, std::string name, float scalemin, float scalemax); //Generate SSBO instances to the Y value of the terrain mesh[0]
 void printMatrix(glm::mat4 matrix, std::string location); //to print matrices to the console 
 void initialise(float x, float y, float z, GLFWwindow* window);
//function for returning shape type as string
std::string GetShapeTypeString(int shapeType);
void SetImGuiStyleColors(ImVec4 bgColor, ImVec4 buttonColor);
void update();//Update character and world data / Also main function for other update functions
void updateLightingData(std::vector<World::Lights>& sceneLights, SSBO lightingSSBO, GLuint shader); // function to update the UI interface with light types and position . uses ssbos
void setimGUILook();//function for setting the default colours for imgui
// Variables to hold user inputs for FBO creation
static bool customViewEnabled = false;  // Checkbox state
static char fboName[64] = ""; // Text buffer for the FBO name
static int fboWidth = 800;    // Default width
static int fboHeight = 600;   // Default height
void setupFramebuffer(const std::string& name, int width, int height, bool customViewEnabled);
//imgui styles

struct ImVec3 { float x, y, z; ImVec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) { x = _x; y = _y; z = _z; } };
std::string OpenFileDialog();//windows api fil dialog
void imgui_easy_theming(ImVec3 color_for_text, ImVec3 color_for_head, ImVec3 color_for_area, ImVec3 color_for_body, ImVec3 color_for_pops)
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.95f);
	//.Colors[ImGuiCol_ChildWindowBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.58f);
	style.Colors[ImGuiCol_Border] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.47f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.21f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	//style.Colors[ImGuiCol_Combo] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	//style.Colors[ImGuiCol_Column] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.32f);
	//style.Colors[ImGuiCol_ColumnHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
//style.Colors[ImGuiCol_ColumnActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.15f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	//style.Colors[ImGuiCol_CloseButton] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.16f);
//	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.39f);
	//style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.43f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(color_for_pops.x, color_for_pops.y, color_for_pops.z, 0.92f);
//	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.73f);
}

void SetupImGuiStyle2()
{
	static ImVec3 color_for_text = ImVec3(236.f / 255.f, 240.f / 255.f, 241.f / 255.f);
	static ImVec3 color_for_head = ImVec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
	static ImVec3 color_for_area = ImVec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
	static ImVec3 color_for_body = ImVec3(94.f / 255.f, 112.f / 255.f, 166.f / 255.f);
	static ImVec3 color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
	imgui_easy_theming(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops);
}
void showFBOControlPanel() {
	bool uniqueView = false;
	if (drawIMGUI) {
		ImGui::Begin("FBO Control Panel");

		// Input field for FBO name
		ImGui::InputText("FBO Name", fboName, sizeof(fboName));

		// Input fields for width and height
		ImGui::InputInt("FBO Width", &fboWidth);
		ImGui::InputInt("FBO Height", &fboHeight);
		ImGui::Checkbox("Enable Custom View", &customViewEnabled);

		// Button to create the FBO
		if (ImGui::Button("Create FBO")) {
			// Create a new FBO based on the user inputs
			std::string name(fboName);
			setupFramebuffer(name, fboWidth, fboHeight, customViewEnabled);
			// Button to create the FBO


		}

		// Button to remove the last FBO
		if (ImGui::Button("Remove Last")) {
			if (!framebuffers.empty()) {
				// Remove the last FBO added to the map
				auto lastIt = std::prev(framebuffers.end());
				framebuffers.erase(lastIt);
			}
			else {
				std::cerr << "No FBOs to remove!" << std::endl;
			}
		}

		// Button to clear all FBOs
		if (ImGui::Button("Clear Views")) {
			framebuffers.clear();
		}

		ImGui::End();
	}
}

//pointer to dynamics world and a vector containing rigid bodies to keep track of
// keep in mind dynamicsWorld also stores the rigid body for the simulation
std::vector<btRigidBody*>* rigidBodyVectorPtr = nullptr;
std::vector<Cube> instVector;

//Terrain and Noise Generation Varaibles
Terrain terrain;


void drawFBOToScreen(const std::string& fboName, GLuint textureID, int width, int height);

void calculateGradients(const std::vector<float>& terrain, int width, int height,
	std::vector<float>& gradientX, std::vector<float>& gradientY);
void createNormalMap(const std::vector<float>& gradientX, const std::vector<float>& gradientY,
	int width, int height, std::vector<glm::vec3>& normalMap);
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

void showFBOControlPanel();
void setupFramebuffer(const std::string& name, int width, int height);

int main();
void drawDebugLighting(std::vector<World::Lights>& sceneLights);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void addCubes(GLuint shader, Mesh mesh);
Cube* findCubeByColour(const glm::vec3 color); //used to access teh cubes properties or ptr to cube
/*int findCubeIndexByColour(const glm::vec3 color);*/ //used to delete items from vector
//SSBO* cubeSSBOptr= nullptr;
int cubesToGenerate; //for IMGUI input
glm::mat4 createCamera(glm::vec3& cameraPosition, glm::vec3& targetPosition, glm::vec3& upVector);
using namespace Assimp;
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>
std::vector<std::shared_ptr<Importer>> importers;
std::shared_ptr<Importer> importer = std::make_shared<Importer>(); //assimp importer
void setupAssimpLogging() {
	// Initialize Assimp logging with verbose output
	Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	Assimp::DefaultLogger::get()->info("Assimp logging initialized.");
}
std::vector<const aiScene*>scenes;
int main()
{
	
	importers.push_back(importer);
	setupAssimpLogging();

	// Use smart pointer for the Assimp Importer
	std::unique_ptr<Assimp::Importer> importer = std::make_unique<Assimp::Importer>();

	// Get the list of supported file extensions
	std::string supportedFormats;
	importer->GetExtensionList(supportedFormats);

	// Print the supported file formats
	std::cout << "Supported file formats: " << supportedFormats << std::endl;

	// Check if FBX is supported by searching in the list of supported formats
	if (supportedFormats.find(".fbx") != std::string::npos) {
		std::cout << "FBX format is supported." << std::endl;
	}
	else {
		std::cout << "FBX format is not supported." << std::endl;
	}

	// Cleanup Assimp logger
	
	   // Cleanup Assimp logger
	Assimp::DefaultLogger::kill();//why is open dialog crashing

// Replace with your model file path
	//const aiScene* scene = aiImportFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);
	MyMesh mesh;
	//MyMesh::VertexHandle vhandle[4];
	//vhandle[0] = mesh.add_vertex(MyMesh::Point(-1, -1, 1));
	//vhandle[1] = mesh.add_vertex(MyMesh::Point(1, -1, 1));
	//vhandle[2] = mesh.add_vertex(MyMesh::Point(1, 1, 1));
	//vhandle[3] = mesh.add_vertex(MyMesh::Point(-1, 1, 1));

	// Load the 3D model
	//const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

	int i;
	///-----initialization_start-----

	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// Create a broadphase interface (as before)
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	// Create a constraint solver (as before)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	// Create the dynamics world with islands enabled
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	// Enable islands
	dynamicsWorld->getDispatchInfo().m_enableSPU = true; // Enable SPU processing if available
	dynamicsWorld->getSolverInfo().m_numIterations = 10; // Increase the number of solver iterations (adjust as needed)

	// Set gravity
	dynamicsWorld->setGravity(btVector3(0, -8.81, 0));

	dynamicsWorldPtr = dynamicsWorld;
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
		//dynamicsWorld->addRigidBody(body);
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
	GLFWmonitor* monitor = monitors[0];//Make thi dynamic or add options?
	GLFWwindow* window = glfwCreateWindow(2560, 1440, "Sevilla Engine",NULL, NULL);
	
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
	glfwSetScrollCallback(window, scroll_callback);

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
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	;
	//ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	setimGUILook();
	//GL Drawer debugger
	PhysDebug debugger;
	
	terrain.shaderPtr = debugger.shaderPtr;
	terrain.dynamicsWorldUniversalPtr = dynamicsWorld;
	//debugger.setDebugMode(btIDebugDraw::DBG_DrawConstraints);  // Draw wireframes of objects
	dynamicsWorld->setDebugDrawer(&debugger);
	debugger.setDebugMode(btIDebugDraw::DBG_DrawWireframe);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debugCallback, nullptr);

	//init water
 	Loader waterLoader;
	waterShader watershade;

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
	unsigned char* imgData = stbi_load("grass2.jpg", &imgWidth, &imgHeight, &colChannels, 3); //Defaut ecndary texture that the terrain texture can blend into
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

SetupImGuiStyle2();

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


	//Setup buffers used for (I think) terrain picking for terrain size editing)
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
	
	customShader defaultShaderCustomShader;
	defaultShaderCustomShader.shaderName = "Default Shader";
	defaultShaderCustomShader.shaderProgramID = *defaultShaderProgramPtr;
	customShaders.push_back(&defaultShaderCustomShader);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error afterpolygon mode " << error << std::endl;

	}
	glClearColor(0.28, 0.2, 0.55, 0.0);
	glEnable(GL_DEPTH_TEST);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after enabling depth test: " << error << std::endl;

	}
	glEnable(GL_STENCIL_TEST);

	int textureUniformLoc = glGetUniformLocation(shaderProgram, "texture1");
	glUniform1i(textureUniformLoc, 0); // 0 corresponds to GL_TEXTURE0

	float rotation = 0.15;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	//std::vector < World::meshInstance> meshInstancesVector;
	//Generate Lights
	World::Lights sun;
	World::Lights lamp1;
	World::Lights stars;
	World::Lights campfire;
	World::Lights campfire2;
	campfire.gpuData.isDirectional = 1.0;
	campfire.gpuData.colour = glm::vec3(1.0, 0.0, 0.0);
	campfire.gpuData.strength = 2;
	campfire.gpuData.direction = glm::vec3(0.0, 1.0, 0.0);
	campfire.gpuData.startingPosition = glm::vec3(2.0, 3.0, 2.0);;

	campfire2.gpuData.isDirectional = 0.0;
	campfire2.gpuData.colour = glm::vec3(1.0, 0.0, 0.0);
	campfire2.gpuData.strength = 2;
	campfire2.gpuData.direction = glm::vec3(1.0, 1.0, 0.0);
	campfire2.gpuData.startingPosition = glm::vec3(2.0, 3.0, 2.0);;
	campfire2.gpuData.position = glm::vec3(2.0, 3.0,2.0);
	sun.gpuData.position = glm::vec3(2.0, 2.0, 1.0); // Example position, adjust as needed
	sun.gpuData.colour = glm::vec3(0.3, 0.1,0.3);   // Example color, adjust as needed
	sun.gpuData.strength = 0.22;
	// Example strength, adjust as needed
	sun.gpuData.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
	lamp1.gpuData.position = glm::vec3(1.0, -3.0, 2.0); // Example position, adjust as needed
	lamp1.gpuData.colour = glm::vec3(1.0, 0.1, 0.0);   // Example color, adjust as needed
	lamp1.gpuData.strength =0.22;                 // Example strength, adjust as needed
	lamp1.gpuData.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
	stars.gpuData.position = glm::vec3(1.0, -3.0, 2.0); // Example position, adjust as needed
	stars.gpuData.colour = glm::vec3(1.0, 1.0, 0.0);   // Example color, adjust as needed
	stars.gpuData.strength = 0.22;
	stars.gpuData.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));
	World::Material material;
	material.ambient = glm::vec3(0.2, 0.2, 0.2); // Example ambient color, adjust as needed
	material.diffuse = glm::vec3(0.5, 0.5, 0.5); // Example diffuse color, adjust as needed
	material.specular = glm::vec3(0.8, 0.8, 0.8); // Example specular color, adjust as needed
	material.shininess =1.0;             // Example shininess, adjust as needed
	material.transparency = 0.75;


	CubeMap skybox;
	skybox.loadCubeMap();
	// Get the location of the Material uniform in the shader program

	//sceneLights.push_back(sun);
	//sceneLights.push_back(stars);
	//sceneLights.push_back(lamp1);
	sceneLights.push_back(campfire);
	sceneLights.push_back(campfire2);
	bool boolDebugLights = true;
	//sceneLights.push_back(stars);
	// Upload the sceneLights data to the shader
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error before binding ssbos: " << error << std::endl;
	}
	std::vector<World::LightGPUData> gpuLightsData(sceneLights.size());

	for (size_t i = 0; i < sceneLights.size(); ++i) {
		gpuLightsData[i] = sceneLights[i].gpuData;  // Extract only the GPU data
	}
	SSBO ssboLighting(0, gpuLightsData.data(), gpuLightsData.size() * sizeof(World::LightGPUData), GL_DYNAMIC_DRAW);
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
	float strengthNL{ 0.16 };
	float sceneExposure{ 0.5 };

	//Generate 100 cube instances
	std::cout << "\size of ptr to rigid body is: " << sizeof(btRigidBody*) << "float: " << sizeof(float);
	std::cout << "\nsize of CubeInstance is:" << sizeof(World::cubeInstance);
	std::cout << "\nsize of World Object is:" << sizeof(worldObject);
	std::cout << "\nsize of GLM mat4:" << sizeof(glm::mat4);
	std::cout << "\nsize of GLM vec3:" << sizeof(glm::vec3);
	std::cout << "\nunsigned int:" << sizeof(unsigned int);
	
	rigidBodyVectorPtr = &rigidBodies;
	for (int i = 0; i < 50; i++) {

		glm::dvec3 instancePosition = glm::dvec3(glm::linearRand(glm::vec3(0.0f), glm::vec3(50.0f)));
		glm::dvec3 instanceRotation = glm::linearRand(glm::vec3(-1.0f), glm::vec3(30.0f));
		glm::dvec3 instanceScale = glm::linearRand(glm::vec3(1.0f), glm::vec3(1.0f));
		glm::dmat4 modelMatrix(1.0f);
		modelMatrix = glm::scale(modelMatrix, instanceScale);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.x), glm::dvec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.y), glm::dvec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.z), glm::dvec3(0.0f, 0.0f, 1.0f));
		
		modelMatrix = glm::translate(modelMatrix, instancePosition);
		// Create cube data
		World::cubeInstance cube;
		cube.modelMatrix = modelMatrix;
		
		// Create rigid body data
		glm::dvec3 cubePosition = glm::dvec3(cube.modelMatrix[3]);
		glm::vec3 scale = glm::vec3(cube.modelMatrix[0][0], cube.modelMatrix[1][1], cube.modelMatrix[2][2]);

		btCollisionShape* boxShape = new btBoxShape(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(cubePosition.x, cubePosition.y, cubePosition.z));

		btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, motionState, boxShape);
		btRigidBody* rigidBody = new btRigidBody(rbInfo);
		//cube.rigidBody = rigidBody;
		dynamicsWorld->addRigidBody(rigidBody);
		rigidBody->setUserPointer(reinterpret_cast<void*>(cube.ID));

		rigidBodies.push_back(rigidBody); // St
		cubesSSBOVector.push_back(cube);

	}

	btScalar cubeSideLength = 10.0f;

	// Create a collision shape for the cube
	btCollisionShape* cubeShape = new btBoxShape(btVector3(cubeSideLength * 0.5f, cubeSideLength * 0.5f, cubeSideLength * 0.5f));
	
	// Define random position, rotation, and scale for the cube
	glm::vec3 instancePosition = glm::linearRand(glm::vec3(-33.0f), glm::vec3(33.0f));
	glm::vec3 instanceRotation = glm::linearRand(glm::vec3(-180.0f), glm::vec3(180.0f));
	glm::vec3 instanceScale = glm::linearRand(glm::vec3(1.0f), glm::vec3(1.0f));

	// Create the model matrix for the cube
	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(modelMatrix, instancePosition);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(instanceRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, instanceScale);

	// Set the initial transformation for the cube
	btTransform cubeTransform;
	cubeTransform.setIdentity();
	cubeTransform.setOrigin(btVector3(instancePosition.x, instancePosition.y, instancePosition.z));

	// Create rigid body for the cube
	btScalar cubeMass = 1.0f;
	bool isCubeDynamic = (cubeMass != 0.f);

	btVector3 cubeLocalInertia(0, 0, 0);
	if (isCubeDynamic)
		cubeShape->calculateLocalInertia(cubeMass, cubeLocalInertia);

	btDefaultMotionState* cubeMotionState = new btDefaultMotionState(cubeTransform);
	btRigidBody::btRigidBodyConstructionInfo cubeRbInfo(cubeMass, cubeMotionState, cubeShape, cubeLocalInertia);
	btRigidBody* cubeBody = new btRigidBody(cubeRbInfo);

	// Add the cube rigid body to the dynamics world
	dynamicsWorld->addRigidBody(cubeBody);
	collisionShapes.push_back(cubeShape);

	// Create cube data
	World::cubeInstance cube;
	cube.modelMatrix = modelMatrix;
	cubesSSBOVector.push_back(cube);
	
	//model matrix for unique ground cube to pass to vertex shader if statement
	
	modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelUniform");
	GLuint isInstancedBool =  glGetUniformLocation(shaderProgram, "isInstanced");
	GLuint isStencilled = glGetUniformLocation(shaderProgram, "isStencil");
	if (boolShowGLErrors) {
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "before terrain " << error << std::endl;



		}
	}

	
	
	//char mapFile[] = "256.raw";

	char map[] = "map.raw";
	terrain.loadHeightMap(map, 16);
	terrain.generateHeightMap();
	if (boolShowGLErrors) {
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at game loop start: " << error << std::endl;



		}
	}
	cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
	if (boolShowGLErrors) {
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at game loop start: " << error << std::endl;



		}
	}
	cubeSSBOptr = &cubeSSBO;
	

	//initalise game / variables and start location
	
	
	//passing character start locations
	//initalise function for engine takes characters XYZ and window.
	initialise(43.0, 53.0, 37.0, window);

		//fps details
	
	static std::vector<double> frameTimes;

	WaterRenderer waterRendererProgram(waterLoader, watershade, projection);
	waterFrameBuffers waterFBOS; 
	globalWaterShader = &watershade.ID;


	Gizmo gizmo;
	gizmo.window = window;
	WaterTile watertile(0, 0, -1);
	WaterTile watertile2(-20, 0,-1); 
	WaterTile watertile3(25, 50, -1);
	WaterTile watertile4(-75, 25, -1);
	WaterTile watertile5(-75, 25, -1);
	WaterTile watertile6(-0, -25, -1);
	WaterTile watertile7(75, 25, -1);
	WaterTile watertile8(35, 0, -1);
	waterTileVector.push_back(watertile);
	waterTileVector.push_back(watertile2);
waterTileVector.push_back(watertile3);
	waterTileVector.push_back(watertile4);
	waterTileVector.push_back(watertile5);
	waterTileVector.push_back(watertile6);
	waterTileVector.push_back(watertile7);
	waterTileVector.push_back(watertile8);
	uimanager = new UIManager(window_width, window_height, true);
	objimporter = new ObjImporter;
	uimanager->addTextureFromFBO(waterFBOS.getReflectionTexture(), 5000, 5000, -0.7,0.85, 0.35, 0.35);
	uimanager->addTextureFromFBO(waterFBOS.getRefractionTexture(), 5000, 5000, 0.4, 0.85, 0.45, 0.45);
//	uimanager->addTextureFromFBO(waterFBOS.getReflectionBufferID(), 100, 100, 0.4, 0.85, 0.45, 0.45);
	waterRendererProgram.reflectionTextureID = waterFBOS.getReflectionTexture();
	waterRendererProgram.refractionTextureID = waterFBOS.getRefractionTexture();
	camera.setViewMatrix();
	WorldGrid worldgrid;
	worldgridPtr = &worldgrid;
	//generate fbos for imgui rendering world in window

	glGenFramebuffers(1, &terrain.eng_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, terrain.eng_fbo);

	// Generate texture to hold color attachment
	glGenTextures(1, &terrain.eng_texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, terrain.eng_texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrain.eng_texColorBuffer, 0);
	
	//v sync
	glfwSwapInterval(1);  // Enable V-Sync for GLFW
	// Generate renderbuffer for depth and stencil
	glGenRenderbuffers(1, &terrain.eng_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, terrain.eng_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, terrain.eng_rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	else { std::cout << "Created fbos etc for rendering world to imgui window"; }
	AnimationController animationController;

	animationControllerPtr = &animationController;
	float lastFrame = 0.0;
	Shader text_shader("shader_text.vert", "shader_text.frag");
	text_shader.use();
	std::vector<Text>screenText;

	//text matrices
	glm::mat4 spinning_mat(1.0f); 

	FT_Library free_type;
	FT_Error error_code = FT_Init_FreeType(&free_type);
	if (error_code)
	{
		std::cout << "\n Error Code: " << error_code << " -- " << "An error occurred with FT_Library";
		int keep_console_open;
		std::cin >> keep_console_open;

	}

	Text text_object1(free_type, window_width, window_height, "1234567890&.-abcdefghijklmnopqrstuvwxyz:_ABCDEFGHIJKLMNOPQRSTUVWXYZ ");
	text_object1.create_text_message("Adding Text to the Seville Engine", 400, 400, "arial.ttf", 30, false);
	text_object1.create_text_message("Welcome", 400, 700, "arial.ttf", 80, false);


	glUniform1i(glGetUniformLocation(text_shader.ID, "alphabet_texture"), 31);
	 
	glm::vec3 RGB = glm::vec3(10.0f, 120.0f, 105.0f);
	unsigned int font_colour_loc = glGetUniformLocation(text_shader.ID, "font_colour");
	glUniform3fv(font_colour_loc, 1, glm::value_ptr(RGB));
	//srand((unsigned)time(NULL)); // Initialise random seed.

	float x_spin = 1.0f / (rand() % 10 + 1); // Generate random number between 1 and 10
	float y_spin = 1.0f / (rand() % 10 + 1);
	float z_spin = 1.0f / (rand() % 10 + 1);
	float spin_speed = (float)(rand() % 5 + 1);

	float spin_vary = 0.0f;
	int spin_dir = 1;

	while (!glfwWindowShouldClose(window))
	{

		//SPIN ANIMATION TEXT MATRIX
		spin_vary += 0.05f * spin_dir;

		if (spin_vary > 6 || spin_vary < 0)
		{
			spin_dir = -spin_dir; // Reverse the spinning direction.

			x_spin = 1.0f / (rand() % 10 + 1);
			y_spin = 1.0f / (rand() % 10 + 1);
			z_spin = 1.0f / (rand() % 10 + 1);
			spin_speed = (float)(rand() % 50 + 1) / 40;
		}
		spinning_mat = glm::rotate(spinning_mat, glm::radians(spin_speed), glm::normalize(glm::vec3(x_spin, y_spin, z_spin)));
		//Get frame time
		currentTime = glfwGetTime();
		// Calculate the elapsed time since the start of the loop
		deltaTime = currentTime - initialTime;
		initialTime = currentTime;
		lastFrame = currentTime;
		frameTimes.push_back(deltaTime);
		while (frameTimes.size() > 100) {
			frameTimes.erase(frameTimes.begin());
		}

		// Calculate FPS
		double fps = 1.0 / deltaTime;

		// Calculate the lowest 1% frame time over the last 10 seconds
		double lowest1PercentFrameTime = 0.0;

		if (frameTimes.size() >= 10) {
			// Sort frame times in ascending order
			std::vector<double> sortedFrameTimes = frameTimes;
			std::sort(sortedFrameTimes.begin(), sortedFrameTimes.end());

			// Calculate the index that corresponds to the lowest 1% frame time
			int index = static_cast<int>(sortedFrameTimes.size() * 0.01);

			// Get the lowest 1% frame time
			lowest1PercentFrameTime = sortedFrameTimes[index];
		}
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		update();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//drawing everywhere / incase this is an FBO issue.

		// Assuming you have the rigid body's position in 'rigidBodyPosition'
		btVector3 rigidBodyPosition = character->getRigidBody()->getCenterOfMassPosition();

		// Calculate the view matrix to focus on the rigid body's position
		glm::mat4 characterViewMatrix = glm::lookAt(glm::vec3(rigidBodyPosition.x(), rigidBodyPosition.y(), rigidBodyPosition.z()),
			glm::vec3(rigidBodyPosition.x(), rigidBodyPosition.y(), rigidBodyPosition.z() + 3.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		if (character->characterActive)
	{
			camera.mPosition = glm::vec3(rigidBodyPosition.x(), rigidBodyPosition.y() + characterCameraYOffset, rigidBodyPosition.z() + 3.0f); // Offset the camera

			// Calculate the view matrix
			camera.mViewMatrix = glm::lookAt(camera.mPosition, glm::vec3(rigidBodyPosition.x(), rigidBodyPosition.y(), rigidBodyPosition.z()), glm::vec3(0.0f, 1.0f, 0.0f));

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		}
		// 
		// 
		//Updates all update functions, including character
		if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_REPEAT)

		{
		if(drawIMGUI)	drawIMGUI = false;
		else if (!drawIMGUI)	drawIMGUI = true;

		}

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

		
		if (boolShowGLErrors) {
			GLenum error;
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error at game loop start: " << error << std::endl;



			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window_width, window_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	//redundant uniform? now 'view' for camera matrix, not viewpos
		GLuint viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos");

		// Set the viewPos uniform to the camera's position
		glm::vec3 cameraPosition = camera.mPosition; // Change this to your camera's position
		glUniform3fv(viewPosLocation, 1, glm::value_ptr(cameraPosition));
		std::vector<std::vector<glm::vec3>> lightPaths(sceneLights.size());

		for (auto& light : sceneLights) {
			if (light.curvePositions.empty() && light.gpuData.isDirectional == 0.0) { // Ensure we only generate if it's empty
				for (int i = 0; i < 10; ++i) {
					float x = static_cast<float>(rand() % 110) - 60; // Random x within [-50, 50]
					float y = static_cast<float>(rand() % 40) + 11;   // Random y within [5, 55]
					float z = static_cast<float>(rand() % 110) - 60; // Random z within [-50, 50]
					light.curvePositions.push_back(glm::vec3(x, y, z)); // Add point to the vector
				}
			}
			if (light.gpuData.isDirectional == 1.0)
			{
				light.curvePositions.clear();
			}
		}

		// Persistent state for movement for all lights
		for (auto& light : sceneLights) {
			// Increment lerp time
			light.lightLerp.currentLerpTime += deltaTime * light.lightLerp.speedMultiplier;

			if (light.curvePositions.size() < 2) continue; // Ensure we have enough points

			// Check if we need to move to the next point
			if (light.lightLerp.currentLerpTime >= light.lightLerp.moveDuration) {
				light.lightLerp.currentLerpTime = 0.0f; // Reset lerp time
				light.lightLerp.currentPoint = (light.lightLerp.currentPoint + 1) % light.curvePositions.size(); // Move to next point
			}


			// Get the next point index
			int nextPoint = (light.lightLerp.currentPoint + 1) % light.curvePositions.size();

			// Calculate interpolation factor (t)
			float t = light.lightLerp.currentLerpTime / light.lightLerp.moveDuration;

			// Lerp between current and next points and update the GPU data position
			light.gpuData.position = glm::lerp(light.curvePositions[light.lightLerp.currentPoint],
				light.curvePositions[nextPoint], t);
		}
		for (size_t i = 0; i < sceneLights.size(); ++i) {
			gpuLightsData[i] = sceneLights[i].gpuData;  // Extract only the GPU data
		}
		//float currentTime = glfwGetTime(); // Get current time in seconds
		//
		//float angle = rotationSpeed * time;

		//float cameraX = center.x + radius * cos(angle);
		//float cameraZ = center.z + radius * sin(angle);

		//camera.mPosition = glm::vec3(cameraX, camera.mPosition.y, cameraZ);
		//camera.setViewMatrix();
		ssboLighting.updateData(gpuLightsData.data(), gpuLightsData.size() * sizeof(World::LightGPUData));

		glUseProgram(shaderProgram);
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
		if (boolDrawUI) {
			 drawUI();
			
		}

		int w, h;

		//glfwGetWindowSize(window, &w, &h);

		
	
		// Input box for the number of cubes
		double totalTimeLast10Seconds = 0.0;
		for (double frameTime : frameTimes) {
			totalTimeLast10Seconds += frameTime;
		}

		// Calculate the average FPS for the last 10 seconds
		double averageFPSLast10Seconds = frameTimes.size() / totalTimeLast10Seconds;
		glm::vec3 caveCenter(10.0f, 5.0f, 10.0f); // Example cave center
		float caveRadius = 125.0f; // Example cave radius
		float caveDepth = 253.0f;
		if (drawIMGUI) {
			ImGui::Begin("Cave");
			if (ImGui::Button("Generate Cave")) {

				// Code to create the cave directly within the button press
				for (size_t i = 0; i < terrain.vertices.size(); i += 3) {
					GLfloat x = terrain.vertices[i];
					GLfloat y = terrain.vertices[i + 1];
					GLfloat z = terrain.vertices[i + 2];

					glm::vec3 vertex(x, y, z);
					float distanceToCave = glm::length(vertex - caveCenter);

					if (distanceToCave < caveRadius) {
						float depth = caveDepth * (1.0f - (distanceToCave / caveRadius));
						vertex.y -= depth;
					}

					terrain.vertices[i] = vertex.x;
					terrain.vertices[i + 1] = vertex.y;
					terrain.vertices[i + 2] = vertex.z;
				}
				glBindBuffer(GL_ARRAY_BUFFER, terrain.VBO);
				glBufferData(GL_ARRAY_BUFFER, terrain.vertices.size() * sizeof(GLfloat), terrain.vertices.data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

			}
			ImGui::End();

			ImGui::Begin("Frame Info");

			ImGui::Text("Frame Time: %.4f ms", deltaTime * 1000.0);
			ImGui::Text("Average FPS (Last 10s): %.1f", averageFPSLast10Seconds);
			ImGui::Text("FPS: %.1f", fps);
			ImGui::Text("Lowest 1%% Frame Time (Last 10s): %.4f ms", lowest1PercentFrameTime * 1000.0);
			ImGui::Text("Lowest 1%% Frame Time (Last 10s) FPS: %.1f", 1.0 / lowest1PercentFrameTime);

			ImGui::End();

		}
		if (drawIMGUI) {
			ImGui::Begin("Camera Location");
			if (ImGui::Button("Snap to World Origin")) {
				glm::mat4 worldOrigin = glm::mat4(1.0f); // Identity matrix
				btRigidBody* player1 = character->getRigidBody();
				btVector3 newPosition(0.0f, 15.0f, 0.0f);

				// Set the new position for the player character's rigid body
				player1->getWorldTransform().setOrigin(newPosition);
				btVector3 newLinearVelocity(0.0f, 0.0f, 0.0f);
				player1->setLinearVelocity(newLinearVelocity);
				if (!player1->isActive()) {
					// Print a message to the console
					ImGui::Text("RigidBody is not active!");

				}
				else
				{
					ImGui::Text("Character Rigid Body Is Active")
						;
				}
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
			ImGui::InputFloat("Camera Offset for Player", &characterCameraYOffset, 0.0, 150);
			// Get the view matrix from your camera

			btTransform transform;
			btRigidBody* charr = character->getRigidBody();
			transform = charr->getWorldTransform();
			// Get the character's current position
			btVector3 position = transform.getOrigin();

			// Display the character's position
			ImGui::Text("Character Position: %.2f, %.2f, %.2f", position.getX(), position.getY(), position.getZ());

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
		}
			glm::mat4 matrix1 = camera.getViewMatrix();
			glm::mat4 matrix2 = camera.getViewMatrix();
			glm::mat4 matrix3 = camera.getViewMatrix();

			ImVec4 color1 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
			ImVec4 color2 = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
			ImVec4 color3 = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue

			// Open a new ImGui window to display matrices
		if (drawIMGUI){
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
		}
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
			ImGui::End();
	//	This code iterates through all the collision objects in your dynamics world, checks if each object is a rigid body, and prints relevant information like position, linear velocity, and angular velocity to the console.If an object is not a rigid body, it prints "Unknown Collision Object Type." You can customize the output and add more data based on your specific needs.



		
		cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
		//ImGui::End();

		if (drawIMGUI) {


			static char message_buffer[256] = "";
			static char font_path_buffer[256] = "arial.ttf";
			static int text_start_x = 100;
			static int text_start_y = 100;
			static int font_size = 24;
			static bool dynamic_static = true;

			ImGui::Begin("Text Message Editor");

			ImGui::InputText("Message", message_buffer, IM_ARRAYSIZE(message_buffer));
			ImGui::InputInt("Start X", &text_start_x);
			ImGui::InputInt("Start Y", &text_start_y);
			ImGui::InputText("Font Path", font_path_buffer, IM_ARRAYSIZE(font_path_buffer));
			ImGui::InputInt("Font Size", &font_size);
			ImGui::Checkbox("Dynamic", &dynamic_static);

			if (ImGui::Button("Create Message")) {
				Text text_object(free_type, window_width, window_height, "1234567890&.-abcdefghijklmnopqrstuvwxyz:_ABCDEFGHIJKLMNOPQRSTUVWXYZ ");
				text_object.create_text_message(message_buffer, text_start_x, text_start_y, font_path_buffer, font_size, false);
				screenText.push_back(text_object);
			}
			if (ImGui::Button("Delete Messages")) {
				screenText.clear();
			}
			ImGui::End();


			ImGui::Begin("Add Lighting");
			ImGui::Selectable("Draw Debug Lights", &boolDebugLights);
			// Control for scene exposure
			if (ImGui::SliderFloat("Exposure", &sceneExposure, 0.01f, 1.0f)) {
				int sceneExposureLocation = glGetUniformLocation(shaderProgram, "exposure");
				glUniform1f(sceneExposureLocation, sceneExposure);
			}

			ImGui::Text("Add Light");
			ImGui::SliderFloat3("Position", glm::value_ptr(positionNL), -40.0f, 40.0f); // Control for new light position
			ImGui::SliderFloat("Strength", &strengthNL, 0.1f, 5.0f);  // Control for new light strength
			ImGui::ColorEdit3("Colour", glm::value_ptr(colourNL));    // Control for new light color

			// Adding a new light to the scene
			if (ImGui::Button("Add Light to Scene")) {
				glUseProgram(shaderProgram);  // Use the shader program before updating uniforms

				// Fill the new light's GPU data
				newLight.gpuData.position = positionNL;
				newLight.gpuData.colour = colourNL;
				newLight.gpuData.strength = strengthNL;
				newLight.gpuData.startingPosition = glm::linearRand(glm::vec3(-5.0f), glm::vec3(5.0f));

				// Add the new light to the sceneLights array
				sceneLights.push_back(newLight);
				 gpuLightsData.push_back(newLight.gpuData); // Add the new light's GPU data

				// Update the shader uniform for the array size
				int arraySizeLocation = glGetUniformLocation(shaderProgram, "arraySize");
				glUniform1i(arraySizeLocation, sceneLights.size());

				// Create a container to store only the GPU data
				std::vector<World::LightGPUData> gpuLightsData(sceneLights.size());
				for (size_t i = 0; i < sceneLights.size(); ++i) {
					gpuLightsData[i] = sceneLights[i].gpuData;  // Copy only the GPU data
				}

				// Use the other shader 
				if (modelLoaderNew.modelNewVector.size() > 0) {
					glUseProgram(modelLoaderNew.shaderProgram->ID);

					// Bind and update the SSBO with only the LightGPUData
					ssboLighting.Bind();
					ssboLighting.updateData(gpuLightsData.data(), gpuLightsData.size() * sizeof(World::LightGPUData));

					// Update the arraySize in the second shader program as well
					arraySizeLocation = glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "arraySize");
					glUniform1i(arraySizeLocation, sceneLights.size());

					// Switch back to the original shader program
					glUseProgram(shaderProgram);
				}
			}

			// Option to remove the last light in the scene
			if (ImGui::Button("Remove Last Light")) {
				if (!sceneLights.empty()) {
					sceneLights.pop_back();
					gpuLightsData.pop_back(); // Add the new light's GPU data

					//for (size_t i = 0; i < sceneLights.size(); ++i) {
					//	gpuLightsData[i] = sceneLights[i].gpuData;  // Extract only the GPU data
					//}
					ssboLighting.updateData(gpuLightsData.data(), gpuLightsData.size() * sizeof(World::LightGPUData));

				}
			}

			ImGui::End();
		}
		if (drawIMGUI) {
			ImGui::Begin("Settings");
			int lightname = 0;


			for (auto& source : sceneLights) {
    // Sun position sliders
    std::string name = "Sun X " + std::to_string(lightname);
    if (ImGui::SliderFloat(name.c_str(), &source.gpuData.position.x, -60.0f, 60.0f)) {
		updateLightingData(sceneLights, ssboLighting, modelLoaderNew.shaderProgram->ID);
    }
    
    name = "Sun Y " + std::to_string(lightname);
    if (ImGui::SliderFloat(name.c_str(), &source.gpuData.position.y, -60.0f, 60.0f)) {
		updateLightingData(sceneLights, ssboLighting, modelLoaderNew.shaderProgram->ID);
    }

    name = "Sun Z " + std::to_string(lightname);
    if (ImGui::SliderFloat(name.c_str(), &source.gpuData.position.z, -60.0f, 60.0f)) {
		updateLightingData(sceneLights, ssboLighting, modelLoaderNew.shaderProgram->ID);
    }

    name = "Strength " + std::to_string(lightname);
    if (ImGui::SliderFloat(name.c_str(), &source.gpuData.strength, 0.1f, 3.0f)) {
		updateLightingData(sceneLights, ssboLighting, modelLoaderNew.shaderProgram->ID);
    }

    name = "Sun Color " + std::to_string(lightname);
    if (ImGui::ColorEdit3(name.c_str(), glm::value_ptr(source.gpuData.colour))) {
		updateLightingData(sceneLights, ssboLighting, modelLoaderNew.shaderProgram->ID);
    }

    // Option to switch between directional and non-directional light
    name = "Directional Light " + std::to_string(lightname);
	bool isDirectional = (source.gpuData.isDirectional != 0.0f);

	if (ImGui::Checkbox(name.c_str(), &isDirectional)) {
		// Update the float value based on the checkbox state
		source.gpuData.isDirectional = isDirectional ? 1.0f : 0.0f;

		// Update lighting data if needed
		updateLightingData(sceneLights, ssboLighting, modelLoaderNew.shaderProgram->ID);
	}
    // If the light is directional, provide sliders to control its direction
    if (source.gpuData.isDirectional == 1.0f) {
        name = "Direction X " + std::to_string(lightname);
        if (ImGui::SliderFloat(name.c_str(), &source.gpuData.direction.x, -1.0f, 1.0f)) {
			updateLightingData(sceneLights, ssboLighting, modelLoaderNew.shaderProgram->ID);
        }

        name = "Direction Y " + std::to_string(lightname);
        if (ImGui::SliderFloat(name.c_str(), &source.gpuData.direction.y, -1.0f, 1.0f)) {
			updateLightingData(sceneLights, ssboLighting, modelLoaderNew.shaderProgram->ID);
        }

        name = "Direction Z " + std::to_string(lightname);
        if (ImGui::SliderFloat(name.c_str(), &source.gpuData.direction.z, -1.0f, 1.0f)) {
            updateLightingData(sceneLights, ssboLighting, modelLoaderNew.shaderProgram->ID);
        }
    }

    lightname++;
}





			int arraySizeLocation = glGetUniformLocation(shaderProgram, "arraySize");
			glUniform1i(arraySizeLocation, sceneLights.size());
			//	glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);s AD

			ImGui::End();
		}//closes imgui on boolen key press drawIMGUI
		// Render pImGui demo window (for testing)
		//ImGui::ShowDemoWindow();
		cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0])* cubesSSBOVector.size());
		dynamicsWorld->updateAabbs();
		dynamicsWorld->stepSimulation(deltaTime, 1);

		// Update cube positions based on physics simulation
		//if (cubesSSBOVector.size() > 0) {
		//	size_t numCubes = cubesSSBOVector.size();

		//	// Ensure there are cubes to process
		//	if (numCubes > 0) {
		//		for (int i = 0; i < numCubes; i++) {
		//			// Check if the rigid body is valid
		//			if (cubesSSBOVector[i].rigidBody) {
		//				btTransform trans;
		//				cubesSSBOVector[i].rigidBody->getMotionState()->getWorldTransform(trans);

		//				// Extract translation
		//				glm::dvec3 newPosition(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());

		//				// Extract rotation (as a quaternion)
		//				btQuaternion rotation = trans.getRotation();
		//				glm::quat newRotation(rotation.w(), rotation.x(), rotation.y(), rotation.z());

		//				// Check if the corresponding rigid body exists in rigidBodies
		//				if (i < rigidBodies.size() && rigidBodies[i]) {
		//					// Extract scale from the collision shape
		//					btVector3 collisionShapeScale = rigidBodies[i]->getCollisionShape()->getLocalScaling();
		//					glm::vec3 newScale(collisionShapeScale.x(), collisionShapeScale.y(), collisionShapeScale.z());

		//					// Create translation matrix
		//					glm::mat4 translationMatrix = glm::translate(glm::dmat4(1.0f), newPosition);

		//					// Create rotation matrix
		//					glm::mat4 rotationMatrix = glm::mat4_cast(newRotation);

		//					// Create scale matrix
		//					glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), newScale); // Use newScale instead of (1.0, 1.0, 1.0)

		//					// Combine translation, rotation, and scale
		//					cubesSSBOVector[i].modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		//				}
		//				else {
		//					// Log a warning or error if the corresponding rigid body is not found
		//					std::cerr << "Warning: Rigid body at index " << i << " is nullptr or out of bounds." << std::endl;
		//				}
		//			}
		//			else {
		//				// Log a warning if the rigid body in cubesSSBOVector is nullptr
		//				std::cerr << "Warning: cubesSSBOVector[" << i << "].rigidBody is nullptr." << std::endl;
		//			}
		//		}
		//	}
		//	else {
		//		// If no cubes are present, clear the rigid bodies and cubesSSBOVector
		//		rigidBodies.clear();
		//		cubesSSBOVector.clear();
		//		// cubeSSBO.clearSSBO(); // Uncomment if necessary
		//	}
		//	// Update SSBO data
		//	cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
		//}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		if (terrain.renderSkyBox) {
			skybox.render(camera.getViewMatrix(), projection); // Use the CubeBox's render function
		}
		
		// Render ImGui UI
	//	debugger.SetMatrices(camera.getViewMatrix(), projection);
		if (boolRigidBody) { dynamicsWorld->debugDrawWorld(); }// If to draw the physics debug wireframes
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		ssboLighting.Bind();
		mousePickingValue = 1;
		GLint mousePickingUniformLocation = glGetUniformLocation(shaderProgram, "mousePicking");
		glUniform1i(mousePickingUniformLocation, mousePickingValue);
	//	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
	//	glViewport(0, 0, window_width, window_height);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		cubeSSBOptr->Bind();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, cubeSSBOptr->ssboID);
		glUniform1i(isInstancedBool, 1);
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 5000);
	if (boolShowGLErrors) {
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: first draw call (picking) " << error;
		}
	}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	
		//cubeTest.draw();
		
		glUniform1i(isInstancedBool, 0);

		
		mousePickingValue = 0;
		mousePickingUniformLocation = glGetUniformLocation(shaderProgram, "mousePicking");
		glUniform1i(mousePickingUniformLocation, mousePickingValue);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glUniform1i(isInstancedBool, 1);
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 5000);
		if (boolShowGLErrors) {
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "\nopenGL Error at: second draw call" << error;
			}
			//std::cout << "\nMain shader program name: " << shaderProgram;
		}
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		cubeSSBOptr->Unbind();
		glUseProgram(shaderProgram);
		glUniform1i(isInstancedBool, 0);
	//	cubeTest.draw();
		glUniform1i(isStencilled, 0);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	
		for (auto& mesh : meshVector) {
		
		//	mesh.SetWorldTransform(modelMatrix);
			mesh->Render(shaderProgram);
			
			//Here the object is drwing correctly using a different 
			//process. I'm rewriting the process
		}
	

		int instancedInt = 0;
		glUseProgram(shaderProgram);
		glUniform1i(isInstancedBool, 1);
		//make sure using correct shader
		//problem that mesh . render doesnt use instancing.
	
		//stencil prep for following render loop
		glEnable(GL_DEPTH_TEST);
		
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glUniform1i(isStencilled, 0);

		for (auto& each : SSBOVector)
		{
			//binds each ssbo which corresponds to the index position (intanceInt of the equivalent mesh)
			//this mesh is then rendered.
			//->Bind();
			instancedMeshVector[instancedInt]->renderInstance(shaderProgram, each->ssboID, each->instanceAmount);
			//std::cout << "\n" << each->ssboID << " is the bound SSBO ID\n";
			instancedInt++;
			//each->Unbind();
		}
	glUniform1i(isStencilled, 1);
		glUniform1i(isInstancedBool, 1);
		//enable stencfil buffer
		//make sure its cleared
		//setuptest//test results and mask.
		//render again // resize objects

	

		glStencilFunc(GL_NOTEQUAL,1, 0xFF);
		glStencilMask(0x00);
		
		instancedInt = 0;
		for (auto& each : SSBOVector)
		{
			//binds each ssbo which corresponds to the index position (intanceInt of the equivalent mesh)
			//this mesh is then rendered.
			//each->Bind();
			instancedMeshVector[instancedInt]->renderInstance(shaderProgram, each->ssboID, each->instanceAmount);
			//std::cout << "\n" << each->ssboID << " is the bound SSBO ID\n";
			instancedInt++;
			//each->Unbind();
		}
		
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);

		glUniform1i(isStencilled, 0);
		glUseProgram(shaderProgram);
	glUniform1i(isInstancedBool, 0);
	//	mesh.Render(*defaultShaderProgramPtr);
		glBindVertexArray(VAO);
		
		
	/*	for (auto item : setDrawLinePositions) {
			std::cout << item << "\n";
		
		}*/
	
	
		int widthreflection = 1000;
		int heightreflection = 1000;
		if (!terrain.init) terrain.initalise();
		instancedInt = 0;


		if (terrain.ready)
		{
				glViewport(0, 0, window_width, window_height);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			waterRendererProgram.render(waterTileVector, camera);
			glUseProgram(debugger.shaderProgram);
			GLint view = glGetUniformLocation(debugger.shaderProgram, "view");
			glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

			projectionLoc = glGetUniformLocation(debugger.shaderProgram, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			//	// Reset the draw buffers to the default (back) buffer
			
			terrain.render();
		
			GLint clipPlaneLocation = glGetUniformLocation(debugger.shaderProgram, "plane");
			 view = glGetUniformLocation(debugger.shaderProgram, "view");
			glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			waterFBOS.bindReflectionFrameBuffer();
			
			glEnable(GL_CLIP_DISTANCE0);
	//	glUseProgram(debugger.shaderProgram);
			//GLint clipPlaneLocation = glGetUniformLocation(debugger.shaderProgram, "plane");
		//	//GLint view = glGetUniformLocation(debugger.shaderProgram, "view");
		glUniform4f(clipPlaneLocation, 0.0f, 1.0f, 0.0f, 1.0f); // Assuming the plane is along the y-axis
			camera.flipPitch();
			camera.setViewMatrix();//send new view presumably for flipped camera
			glUseProgram(debugger.shaderProgram);
			 view = glGetUniformLocation(debugger.shaderProgram, "view");
			glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

			projectionLoc = glGetUniformLocation(debugger.shaderProgram, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//	//	printMatrix(camera.mViewMatrix, "first iteration ");
	//	instancedInt = 0;
		//glUseProgram(shaderProgram);
		//glUniform1i(isInstancedBool, 0);
		//	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//	/*for (auto& each : SSBOVector)
		//	{
		//		instancedMeshVector[instancedInt]->renderInstance(shaderProgram, each->ssboID, each->instanceAmount);
		//		instancedInt++;
		//	}*/
		//	waterFBOS.bindReflectionFrameBuffer();
		////	instancedInt = 0;
			glUseProgram(debugger.shaderProgram);
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "GL error: " << error << std::endl;
				std::string err = "Gl Error in main render terrain function main.cpp" + std::to_string(error);
				logger.AddLog(err, ImGuiLogger::LogType::OpenGL);
			}

		//	//	glUniform1i(isInstancedBool, 0);
		//	//glUseProgram(debugger.shaderProgram);
		//	waterFBOS.bindReflectionFrameBuffer();
					watershade.setMatrixUniform(debugger.shaderProgram, camera.mViewMatrix);
			glUseProgram(debugger.shaderProgram);
			
			 view = glGetUniformLocation(debugger.shaderProgram, "view");
			glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

			projectionLoc = glGetUniformLocation(debugger.shaderProgram, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		//			//RENDER TERRAIN FOR REFLECTION FBO
		terrain.render();
		//	glUseProgram(debugger.shaderProgram);
		//	//ImGui::Begin("screenshot fbo");


			waterFBOS.bindRefractionFrameBuffer();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			camera.flipPitch();
				camera.setViewMatrix();
			glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

			clipPlaneLocation = glGetUniformLocation(debugger.shaderProgram, "plane");
			glUniform4f(clipPlaneLocation, 0.0f, -1.0f, 0.0f, 0.0f); // Assuming the plane is along the y-axis

		//	instancedInt = 0;
		//	glUseProgram(shaderProgram);
		//	//glUniform1i(isInstancedBool, 1);
		//	////	glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//	//for (auto& each : SSBOVector)
		//	//{
		//	//	instancedMeshVector[instancedInt]->renderInstance(shaderProgram, each->ssboID, each->instanceAmount);
		//	//	instancedInt++;
		//	//}
		// 
		//	//waterFBOS.bindRefractionFrameBuffer();
		//	//	glUniform1i(isInstancedBool, 0);
			glDisable(GL_CLIP_DISTANCE0);

			
			glUseProgram(debugger.shaderProgram);
			 view = glGetUniformLocation(debugger.shaderProgram, "view");
			glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

			projectionLoc = glGetUniformLocation(debugger.shaderProgram, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//	waterFBOS.bindRefractionFrameBuffer();
			terrain.render();
		//	glUseProgram(debugger.shaderProgram);
		//	waterFBOS.unbindCurrentFrameBuffer();
		

			//	CC("before loading view in render", "debug");
			glUseProgram(debugger.shaderProgram);
			//terrain.render();
			glUseProgram(debugger.shaderProgram);
			//Uncomment to see the FBOs for the reflection and refractions of the water 
			//uimanager->renderUI();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glUseProgram(debugger.shaderProgram);
			terrain.render();
			GLint currentFramebuffer;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);
			std::cout << "\ncurrent frame buffer:" << currentFramebuffer;
			glBindFramebuffer(GL_FRAMEBUFFER, terrain.terrainPickFBO);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "Framebuffer is not complete after MAIN terrain.render!" << std::endl;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, terrain.terrainPickFBO);
			GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT1 };
			glDrawBuffers(1, drawBuffers);
			glUseProgram(debugger.shaderProgram);
			glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			GLint projectionLocation = glGetUniformLocation(debugger.shaderProgram, "projection");
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

			GLint editModeLocation = glGetUniformLocation(*terrain.shaderPtr, "terrainEditMode");
			glUniform1i(editModeLocation, terrainPickingSwitch);
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}
			terrain.render();//rendering terrain for displaying edit mode
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}
			/*	GLenum defaultDrawBuffer = GL_BACK;
				glDrawBuffer(defaultDrawBuffer);
				glReadBuffer(defaultDrawBuffer);*/
				//SEEMS TO BE an error when switching buffers, lets debug with this:
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}

			//lets see if we still error once we have terrain setup and use the fbo
			glBindFramebuffer(GL_FRAMEBUFFER, terrain.terrainPickFBO);
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}
			glUseProgram(debugger.shaderProgram);
			terrain.terrainEditUI(window);

			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);
			std::cout << "\ncurrent frame buffer after switching:" << currentFramebuffer;
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}
		

		

			 	glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			showFBOControlPanel();
		//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//

			for (const auto& entry : framebuffers) {
				const FramebufferObject& fboObj = entry.second;
				drawFBOToScreen(fboObj.name, fboObj.texColorBuffer, fboObj.width, fboObj.height);
			}

		//	for (auto& model : models) {
		//		//glViewport(0, 0, window_width, window_height);
		//		model.draw(model.shaderProgram);

		//		//lets just draw in one location as before, when it worked fine!!
		//	}
		//	
		}
		//glUseProgram(shaderProgram);

	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		debugger.SetMatrices(camera.getViewMatrix(), projection);
	
		// Create and bind a texture to capture framebuffer content
		
		//modelLoaderNew.showMaterialEditor(modelLoaderNew.modelNewVector, modelLoaderNew.activeModelIndex, modelLoaderNew.activeSubMeshIndex);
		// Render the texture to an ImGui window
		if (boolShowGrid)
		{
			worldgrid.draw(projection);
		

		}
		

		//For every model within modelNewVector, we call render and pass in the shader and the current time 
		if (modelLoaderNew.modelNewVector.size() > 0) {
			modelLoaderNew.Render(1, deltaTime);
			
			//worldgrid.draw(projection);
		
			
		}
	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		if (drawIMGUI) {
			logger.Draw("Debug Console", &logger.logWindowOpen);
		}
		if (boolDebugLights && modelLoaderNew.modelNewVector.size() > 0) {
			drawDebugLighting(sceneLights);
			glUseProgram(0);
		}

		text_shader.use();
		unsigned int view_mat_text_loc = glGetUniformLocation(text_shader.ID, "view");
		unsigned int proj_mat_text_loc = glGetUniformLocation(text_shader.ID, "projection");
		unsigned int anim_text_loc = glGetUniformLocation(text_shader.ID, "animate");
		glUniformMatrix4fv(view_mat_text_loc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		glUniformMatrix4fv(proj_mat_text_loc, 1, GL_FALSE, glm::value_ptr(projection));

		glUniformMatrix4fv(anim_text_loc, 1, GL_FALSE, glm::value_ptr(spinning_mat)); 
		text_object1.draw_messages(0);	
		text_object1.draw_messages(1);

		for (int i = 0; i < screenText.size(); i++) {
			screenText[i].draw_messages(0);
				}

		//text_object1.draw_alphabets();  
		gizmo.Draw();
			ImGui::Render();
			//boolean is responsible for only drawing the UI once.
			 UIdrawn = false;
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		//debugger.SetMatrices(view, projection);
		//dynamicsWorld->setDebugDrawer(&debugger);
			// 		dynamicsWorld->debugDrawWorld();

		// Swap buffers
		glfwSwapBuffers(window);
		/*if (boolShowGLErrors) {
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "\nopenGL Error at: before poll events";
			}
		}*/
		glfwPollEvents();
	/*	if (boolShowGLErrors) {
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "\nopenGL Error at: after poll events";
			}
		}*/
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

void drawDebugLighting(std::vector<World::Lights>& sceneLights)
{
	for (const auto& light : sceneLights) {
		glUseProgram(modelLoaderNew.shaderProgram->ID);
		Sphere lightSphere(2.0, 36, 18);
		glm::mat4 model = glm::translate(glm::mat4(1.0f), light.gpuData.position); // Create model matrix for the sphere
		glUniform3fv(glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "material.diffuseColor"), 1, glm::value_ptr(light.gpuData.colour)); // Pass light color
		glUniform3fv(glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "material.specularColor"), 1, glm::value_ptr(light.gpuData.colour)); // Pass light color
		//glUniform3fv(glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "material.emissiveColor"), 1, glm::value_ptr(light.colour)); // Pass light color

		glUniformMatrix4fv(glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix())); // Set model matrix

		glUniformMatrix4fv(glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "model"), 1, GL_FALSE, glm::value_ptr(model)); // Set model matrix
		glUniform1f(glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "material.transparency"), 0.3f); // Pass transparency

		lightSphere.draw(); // Draw the sphere at the light's position
	}
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width,height, 0, GL_RGB, GL_FLOAT, NULL); // Update your global height
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	std::cout << "\n\ncolorTexture  buffer ID is: " << colorTexture << "\n";
	if (boolShowGLErrors) {
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;
			std::cout << "OpenGL Error at fbo callback glteximage2D: " << error << std::endl;



		}
	}
	// Update the depth renderbuffer attachment
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderBuffer);
	std::cout << "\nDepth render buffer ID is: "<< depthrenderBuffer << "\n";
	if (boolShowGLErrors) {
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at fbo callback at renderbufferstorage depth buffer: " << error << std::endl;
			std::cout << "OpenGL Error at fbo callback at renderbufferstorage depth buffer: " << error << std::endl;


		}
	}
	// Check framebuffer completeness (for debugging)
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete after resize!" << std::endl;
	}


	//bind frame buffer and 2 colour attachments for picking terrain below (attach 1 used for terrain vertices IDs)
	glBindFramebuffer(GL_FRAMEBUFFER, terrain.terrainPickFBO);

// Set viewport
glViewport(0, 0, width, height);


// Bind texture for GL_COLOR_ATTACHMENT0 (assuming colorTexture is defined somewhere)
glBindTexture(GL_TEXTURE_2D, terrain.terrainPickTexture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);;
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrain.terrainPickTexture, 0);

// Check framebuffer completeness
if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    // Handle error, framebuffer is not complete
    std::cout << "Framebuffer not complete!" << std::endl;
}


glBindTexture(GL_TEXTURE_2D, terrain.terrainPickPickingBuffer);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);;
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, terrain.terrainPickPickingBuffer, 0);
while ((error = glGetError()) != GL_NO_ERROR) {
	std::cout << "OpenGL Error after setting up Terrain FBO textures: " << error << std::endl;

}
if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	std::cout << "Framebuffer is not complete after resize!" << std::endl;
}

// Unbind framebuffer
glBindFramebuffer(GL_FRAMEBUFFER, 0);

// Optional: Check for OpenGL errors
if (boolShowGLErrors) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << std::endl;
    }
}
	// Unbind obje	cts
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (boolShowGLErrors) {
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at fbo callback fbo callback bind to default: " << error << std::endl;
			std::cout << "OpenGL Error at fbo callback fbo callback bind to default: " << error << std::endl;



		}
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

	if (drawIMGUI) {



		// Declare the uniform location in your rendering code

		// ...

		// In your rendering loop
		glUseProgram(*defaultShaderProgramPtr);

		// Update the draw distance uniform


		drawShaderManager();
		ImGui::Begin("SSBO Map Viewer");
		if (ImGui::Button("Clear SSBOs")) {
			SSBOVector.clear();
			//ssbo
			mapSSBOMeshInstanceVector.clear();
			meshNameBufferInstanced.clear();
			instancedMeshVector.clear();
		}
		// First Loop
		int ii = 0;
		//bool showSecondDropdown = false;
		 // Variable to store the selected SSBO

		if (ImGui::BeginCombo("##KeyCombo", "Select SSBO")) {
			// Iterate through the SSBO objects found in mapSSBOMeshInstanceVector


			for (auto& keyValuePair : mapSSBOMeshInstanceVector) {
				ii++;

				// Display a selectable item for each SSBO object
				bool isSelected = (selectedSSBO == &keyValuePair.first);
				if (ImGui::Selectable(keyValuePair.first.SSBOName.c_str(), isSelected)) {
					// Handle item selection, if needed
					tempSelectedSSBO = &keyValuePair.first;
					showSecondDropdown = true; // Set the flag to true when an item is selected
				}
			}
			if (tempSelectedSSBO != nullptr) {
				selectedSSBO = const_cast<SSBO*>(tempSelectedSSBO);

			}
			ImGui::EndCombo();
		}
		//		mapSSBOMeshInstanceVector[
				// Second Loop
		if (showSecondDropdown) {
			//int jj = 0;
			//if (ImGui::BeginCombo("##SecondCombo", "SSBO Items")) {
			//	// Find the iterator associated with the selected SSBO
			//	auto it = mapSSBOMeshInstanceVector.find(*selectedSSBO);

			//	// Check if the key was found
			//	if (it != mapSSBOMeshInstanceVector.end()) {
			//		// Iterate through the vector associated with the selected SSBO
			//		for (const auto& vectorItem : it->second) {
			//			std::string label = std::to_string(vectorItem.ID); // Use SSBOID as the label
			//			// Display a selectable item for each vector element
			//			bool isSelected = (jj == intSelectedmapSSBOMeshInstance);
			//			if (ImGui::Selectable(label.c_str(), isSelected)) {
			//				// Handle item selection, if needed
			//				//selectedMeshInstance = vectorItem.modelMatrix
			//				//mapSSBOMeshInstanceVector[
			//			}
			//			jj++;
			//		}
			//	}

			//	ImGui::EndCombo();
			//}


			int kk = 0;

			if (ImGui::BeginCombo("##ThirdCombo", "SSBO Items - Direct Vector Access")) {
				std::vector<World::cubeInstance> vectorToIterate = selectedSSBO->getDataVector();

				for (int idx = 0; idx < vectorToIterate.size(); ++idx) {
					const auto& vectorItem = vectorToIterate[idx];
					std::string label = std::to_string(vectorItem.ID) + ": " + selectedSSBO->SSBOName;
					boolSSBOCombo3isSelected = (idx == intSelectedmapSSBOMeshInstance);

					if (ImGui::Selectable(label.c_str(), boolSSBOCombo3isSelected)) {
						// Update the selected index
						intSelectedmapSSBOMeshInstance = idx;
						std::cout << "Option Selected is:" << idx << ": " << intSelectedmapSSBOMeshInstance << std::endl;

						// Copy the model matrix to the global matrix
						globalModelMatrix = vectorItem.modelMatrix;

						// Assuming quaternionRotation is in radians, convert it to degrees before passing to ImGui

						// Extract translation, rotation, and scale components

					}
				}


				ImGui::EndCombo();
			}

		}

		glm::vec3 translation, rotation, scale;
		glm::quat quaternionRotation; // for rotation
		glm::vec3 eulerRotationDegrees = glm::degrees(glm::eulerAngles(quaternionRotation));

		// Decompose the model matrix
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(globalModelMatrix, scale, quaternionRotation, translation, skew, perspective);

		// Update ImGui sliders based on the global matrix
	// Translation slider
		//ImGui::SliderFloat3("Translation", glm::value_ptr(translation), -1.0f, 1.0f);
		if (ImGui::SliderFloat3("Translation", &translation[0], -2.0f, 2.0f)) {
			selectedSSBO->setTranslation(intSelectedmapSSBOMeshInstance, translation);
		}

		// Rotation slider
		if (ImGui::SliderFloat3("Rotation", &rotation[0], -180.0f, 180.0f)) {
			selectedSSBO->setRotation(intSelectedmapSSBOMeshInstance, rotation);
		}

		// Scale slider
		if (ImGui::SliderFloat3("Scale", &scale[0], 0.1f, 2.0f)) {
			selectedSSBO->setScale(intSelectedmapSSBOMeshInstance, scale);
		}

		// Update globalModelMatrix with the latest values
		globalModelMatrix = glm::translate(glm::mat4(1.0f), translation) *
			glm::mat4_cast(glm::quat(glm::radians(rotation))) *
			glm::scale(glm::mat4(1.0f), scale);

		// Console information
		/*std::cout << "\nTranslation: (" << translation.x << ", " << translation.y << ", " << translation.z << ")\n";
		std::cout << "Rotation (Euler): (" << eulerRotationDegrees.x << ", "
			<< eulerRotationDegrees.y << ", "
			<< eulerRotationDegrees.z << ")\n";
		std::cout << "Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << ")\n";
		*///DOCS:: SSBO instance model matrices edit tools

		//	if (customShaders.size() > 0) {
		//		shaderIDStrings.clear();
		//		for (const auto& shader : customShaders) {
		//			shaderIDStrings.push_back(std::to_string(shader->shaderProgramID));
		//		}

		//		std::vector<const char*> shaderIDs;
		//		for (const auto& idString : shaderIDStrings) {
		//			shaderIDs.push_back(idString.c_str());
		//		}

		//		ImGui::Combo("Custom Shader ID", &selectedShaderID2, shaderIDs.data(), shaderIDs.size());

		//		if (ImGui::Button("Change Shader") && selectedShaderID2 != -1 && selectedMeshIndex != -1 && selectedMeshIndex < meshVector.size()) {
		//			meshVector[selectedMeshIndex]->shaderProgram = customShaders[selectedShaderID2]->shaderProgramID;
		//			meshVector[selectedMeshIndex]->customShaderProgramID = customShaders[selectedShaderID2]->shaderProgramID;

		//		}
		//		// This line was missing

		//	}



		//	ImGui::Text(std::to_string(meshVector[selectedMeshIndex]->customShaderProgramID).c_str());
		//	if (ImGui::Button("Reset Transformations")) {
		//		// Reset transformations to identity matrix
		//		meshVector[selectedMeshIndex]->SetScale(glm::vec3(1.0f));
		//		meshVector[selectedMeshIndex]->SetRotation(glm::vec3(0.0f));
		//		meshVector[selectedMeshIndex]->SetTranslation(glm::vec3(0.0f));
		//	}

		//	if (ImGui::SliderFloat("Scale X", &meshVector[selectedMeshIndex]->scale.x, 0.1f, 10.0f)) {
		//		meshVector[selectedMeshIndex]->SetScale(meshVector[selectedMeshIndex]->scale);
		//	}

		//	if (ImGui::SliderFloat("Scale Y", &meshVector[selectedMeshIndex]->scale.y, 0.1f, 10.0f)) {
		//		meshVector[selectedMeshIndex]->SetScale(meshVector[selectedMeshIndex]->scale);
		//	}

		//	if (ImGui::SliderFloat("Scale Z", &meshVector[selectedMeshIndex]->scale.z, 0.1f, 10.0f)) {
		//		meshVector[selectedMeshIndex]->SetScale(meshVector[selectedMeshIndex]->scale);
		//	}

		//	// For SetRotation
		//	if (ImGui::SliderFloat("Rotation X", &meshVector[selectedMeshIndex]->rotation.x, -180.0f, 180.0f)) {
		//		meshVector[selectedMeshIndex]->SetRotation(meshVector[selectedMeshIndex]->rotation);
		//	}

		//	if (ImGui::SliderFloat("Rotation Y", &meshVector[selectedMeshIndex]->rotation.y, -180.0f, 180.0f)) {
		//		meshVector[selectedMeshIndex]->SetRotation(meshVector[selectedMeshIndex]->rotation);
		//	}

		//	if (ImGui::SliderFloat("Rotation Z", &meshVector[selectedMeshIndex]->rotation.z, -180.0f, 180.0f)) {
		//		meshVector[selectedMeshIndex]->SetRotation(meshVector[selectedMeshIndex]->rotation);
		//	}

		//	// For SetTranslation
		//	if (ImGui::InputFloat("Translation X", &meshVector[selectedMeshIndex]->translation.x)) {
		//		meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
		//	}

		//	if (ImGui::InputFloat("Translation Y", &meshVector[selectedMeshIndex]->translation.y)) {
		//		meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
		//	}

		//	if (ImGui::InputFloat("Translation Z", &meshVector[selectedMeshIndex]->translation.z)) {
		//		meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
		//	}
		//	if (ImGui::SliderFloat("Translation X2", &meshVector[selectedMeshIndex]->translation.x, -800.0f, 800.0f)) {
		//		meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
		//	}

		//	if (ImGui::SliderFloat("Translation Y2", &meshVector[selectedMeshIndex]->translation.y, -800.0f, 800.0f)) {
		//		meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
		//	}

		//	if (ImGui::SliderFloat("Translation Z2", &meshVector[selectedMeshIndex]->translation.z, -800.0f, 800.0f)) {
		//		meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
		//	}

		ImGui::End();
	}
	////	ImGui::EndCombo();





	//	ImGui::End();

		//
	if (drawIMGUI) {
		ImGui::Begin("Create SSBO for Mesh");

		ImGui::InputText("Mesh Name", &meshNameBufferInstanced[0], 10000);
		ImGui::InputInt("Mesh Count", &meshCountInstanced);
		ImGui::InputFloat("Scale Min", &scalemin);
		ImGui::InputFloat("Max", &scalemax);
		ImGui::Text("Import Mesh onto landscape (Y Axis)");
		if (ImGui::Button("Create SSBO For Terrain Y")) {


			Mesh* newInstancedMesh;
			newInstancedMesh = new Mesh;
			newInstancedMesh->meshName = meshNameBufferInstanced; // Set the mesh name
			newInstancedMesh->shaderProgram = 3;
			newInstancedMesh->customShaderProgramID = 3;
			if (customShaders.size() == 0)
				newInstancedMesh->customShaderProgramID = 3;
			else
				newInstancedMesh->customShaderProgramID = 3;

			newInstancedMesh->loadMesh(meshNameBufferInstanced, newInstancedMesh->customShaderProgramID);
			newInstancedMesh->cameraPtr = &camera;


			SSBO* instancedSSBO;
			instancedSSBO = new SSBO(2, meshInstancesVector.data(), sizeof(meshInstancesVector[0]) * meshInstancesVector.size(), GL_DYNAMIC_DRAW);
			instancedSSBO->instanceAmount = meshCountInstanced;
			instancedSSBO->instancedMesh = newInstancedMesh;
			SSBOVector.push_back(instancedSSBO);


			//if name is wrong causes errors.
			instancedMeshVector.push_back(newInstancedMesh);


			generateSSBOInstanceToY(terrain.heightmapData.heights, terrain.size, meshCountInstanced, 0, 0, newInstancedMesh->meshName, scalemin, scalemax);
		}
		if (ImGui::Button("Create SSBO"))
		{

			Mesh* newInstancedMesh;
			newInstancedMesh = new Mesh;
			newInstancedMesh->meshName = meshNameBufferInstanced; // Set the mesh name
			newInstancedMesh->shaderProgram = 3;
			newInstancedMesh->customShaderProgramID = 3;
			if (customShaders.size() == 0)
				newInstancedMesh->customShaderProgramID = 3;
			else
				newInstancedMesh->customShaderProgramID = 3;

			newInstancedMesh->loadMesh(meshNameBufferInstanced, newInstancedMesh->customShaderProgramID);
			newInstancedMesh->cameraPtr = &camera;


			SSBO* instancedSSBO;
			instancedSSBO = new SSBO(2, meshInstancesVector.data(), sizeof(meshInstancesVector[0]) * meshInstancesVector.size(), GL_DYNAMIC_DRAW);
			instancedSSBO->instanceAmount = meshCountInstanced;
			instancedSSBO->instancedMesh = newInstancedMesh;
			SSBOVector.push_back(instancedSSBO);


			//if name is wrong causes errors.
			instancedMeshVector.push_back(newInstancedMesh);


			generateSSBOInstance(meshCountInstanced, newInstancedMesh->meshName);

			//need to load  mesh into new vector (pointer)
			// need to create instances (do random positions based on amount first // then make it Y relative for grass etc)
			// // that should be with meshinstance in worldobjects
			// then add instances to vector in loop, then can create the ssbo.
			// then add the ssbo to a vector of them. need to connect the mesh to the ssbo mesh. indexing across both vectors.
			//	SSBO ssboLighting(0, sceneLights.data(), sizeof(sceneLights[0]) * sceneLights.size(), GL_DYNAMIC_DRAW);
			//SSBO ssboMaterial(1, &material, sizeof(material), GL_DYNAMIC_DRAW);

			cubeSSBO = SSBO(2, cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size(), GL_DYNAMIC_DRAW);

			// Assuming you have access to the SSBO class and the necessary functions
			// Here, you can create the SSBO for the entered mesh name and count
			// You can use the 'mesh_name' and 'mesh_count' variables for this purpose
			// Example:
			// SSBO newSSBO(bindingIndex, data, dataSize, useage);
			// newSSBO.Bind(); // Bind the SSBO
			// ...

			// Placeholder message for demonstration
			std::cout << "\nSSBO created for mesh %s with count: " << meshNameBufferInstanced << " " << meshCountInstanced;
		}

		ImGui::End();

		//ImGui::Begin("Edit SSBOs");

		// Assuming 'mesh.scene' is the Assimp root node

		ImGui::Begin("Assimp Scene Information");
		//char file = fileNameBuffer[0];
		//char meshName = meshNameBuffer[0];
		ImGui::InputText("File Name", &fileNameBuffer2[0], 10000);
		ImGui::InputText("Mesh Name", &meshNameBuffer[0], 10000);



		if (customShaders.size() > 0) {
			std::vector<std::string> shaderIDs; // Use std::string instead of const char*

			for (const auto& shader : customShaders) {
				shaderIDs.push_back(std::to_string(shader->shaderProgramID)); // Store the strings themselves
			}

			// Convert the vector of std::strings to a vector of const char*
			std::vector<const char*> shaderIDPointers;
			for (const auto& id : shaderIDs) {
				shaderIDPointers.push_back(id.c_str());
			}

			ImGui::Combo("Custom Shader ID", &selectedShaderID, shaderIDPointers.data(), shaderIDPointers.size());
		}

		ImGui::Checkbox("Override Load Path to Current", &boolPathOverride);
		ImGui::InputText("Mesh Path Override", &stringMeshPathOverride[0], 10000);
		if (ImGui::Button("Load Model") && !fileNameBuffer2.empty() && !meshNameBuffer.empty()) {
			Mesh* newMesh;
			newMesh = new Mesh;
			newMesh->meshName = meshNameBuffer; // Set the mesh name

			if (customShaders.size() == 0)
				newMesh->customShaderProgramID = *defaultShaderProgramPtr;
			else
				newMesh->customShaderProgramID = customShaders[selectedShaderID]->shaderProgramID;

			newMesh->loadMesh(fileNameBuffer2, newMesh->customShaderProgramID);
			newMesh->cameraPtr = &camera;

			//if name is wrong causes errors.
			meshVector.push_back(newMesh);
			if (selectedMeshIndex == -1)
				selectedMeshIndex = 0; //when all meshes are deleted it is set to -1, when we load a new one here, it should be set to the first
			// Clear the input fields after loading

			// Update the meshNames vector
			Mesh::meshNames.push_back(meshNameBuffer); // Use emplace_back to construct the string directly


		}
	}
	if (drawIMGUI) {
		showLoadModelWindow(&loadModelTwo);
		worldgridPtr->drawGridUI();

		ImGui::End();
	}
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error after showload mdoel window: " << error << std::endl;
	}
	//if (!mesh.scenePtr) {
	//	//std::cout << "Assimp scene is not loaded." << std::endl;
	//	ImGui::Text("Assimp scene is not loaded.");

	//}
	//else {
	//	const aiScene* scene = mesh.scenePtr;

	//	bool hasTextures = scene->HasTextures();
	//	bool hasMaterials = scene->HasMaterials();

	//	if (hasTextures) {
	//		std::cout << "Scene has textures." << std::endl;
	//		ImGui::Text("Scene has textures.");
	//	}
	//	else {
	//		std::cout << "Scene does not have textures." << std::endl;
	//		ImGui::Text("Scene does not have textures.");
	//	}

	//	if (hasMaterials) {
	//		std::cout << "Scene has materials." << std::endl;
	//		ImGui::Text("Scene has materials.");
	//	}
	//	else {
	//		std::cout << "Scene does not have materials." << std::endl;
	//		ImGui::Text("Scene does not have materials.");
	//	}

	//	int numModels = scene->mNumMeshes;
	//	std::cout << "Number of Models/Meshes: " << numModels << std::endl;
	//	ImGui::Text("Number of Models/Meshes: %d", numModels);

	//	for (int i = 0; i < numModels; ++i) {
	//		const aiMesh* paiMesh = scene->mMeshes[i];

	//		if (!paiMesh) {
	//			std::cout << "Invalid mesh at index: " << i << std::endl;
	//			ImGui::Text("Invalid mesh at index: %d", i);
	//			continue;
	//		}

	//		ImGui::Separator();
	//		ImGui::Text("Model %d", i + 1);

	//		ImGui::Text("Number of Vertices: %d", paiMesh->mNumVertices);
	//		//ImGui::Text("Base Vertex: %d", paiMesh->mBaseVertex);
	//	}
	//}
	//ImGui::End();

		if (drawIMGUI) {
			ImGui::Begin("Mesh Properties");
			if (!meshVector.empty()) {
				std::cout << "\nindex: " << selectedMeshIndex << "Vector size: " << meshVector.size() << "before Delete\n";
				ImGui::Text("Selected Mesh Index number is: %d", selectedMeshIndex);
				if (ImGui::Button("Delete Mesh")) {
					std::cout << "pressed delete\n";
					if (selectedMeshIndex >= 0 && selectedMeshIndex < meshVector.size()) {
						std::cout << "\nindex: " << selectedMeshIndex << "Vector size: " << meshVector.size() << "before erase\n";
						if (meshVector.size() == 1) {
							meshVector.clear();
							Mesh::meshNames.clear();
						}
						else {
							meshVector.erase(meshVector.begin() + selectedMeshIndex);
							Mesh::meshNames.erase(Mesh::meshNames.begin() + selectedMeshIndex);
						}
						std::cout << "\nindex: " << selectedMeshIndex << "Vector size: " << meshVector.size() << "after .erase\n";
						if (selectedMeshIndex >= meshVector.size()) {
							selectedMeshIndex = meshVector.empty() ? -1 : meshVector.size() - 1;
						}
					}
				}



				if (meshVector.size() > 0) {
					std::cout << "\nindex: " << selectedMeshIndex << "Vector size: " << meshVector.size() << "after selectedmesh if\n";
					// Dropdown for selecting a mesh
					if (ImGui::BeginCombo("Select Mesh", selectedMeshIndex >= 0 ? Mesh::meshNames[selectedMeshIndex].c_str() : "-")) {
						for (int i = 0; i < Mesh::meshNames.size(); ++i) {
							bool isSelected = (i == selectedMeshIndex);
							if (ImGui::Selectable(Mesh::meshNames[i].c_str(), isSelected)) {
								selectedMeshIndex = i;
							}
							if (isSelected) {
								ImGui::SetItemDefaultFocus(); // Set the default selection
							}
						}
						ImGui::EndCombo();

					}

					if (customShaders.size() > 0) {
						shaderIDStrings.clear();
						for (const auto& shader : customShaders) {
							shaderIDStrings.push_back(std::to_string(shader->shaderProgramID));
						}

						std::vector<const char*> shaderIDs;
						for (const auto& idString : shaderIDStrings) {
							shaderIDs.push_back(idString.c_str());
						}

						ImGui::Combo("Custom Shader ID", &selectedShaderID2, shaderIDs.data(), shaderIDs.size());

						if (ImGui::Button("Change Shader") && selectedShaderID2 != -1 && selectedMeshIndex != -1 && selectedMeshIndex < meshVector.size()) {
							meshVector[selectedMeshIndex]->shaderProgram = customShaders[selectedShaderID2]->shaderProgramID;
							meshVector[selectedMeshIndex]->customShaderProgramID = customShaders[selectedShaderID2]->shaderProgramID;

						}
						// This line was missing

					}



					ImGui::Text(std::to_string(meshVector[selectedMeshIndex]->customShaderProgramID).c_str());
					if (ImGui::Button("Reset Transformations")) {
						// Reset transformations to identity matrix
						meshVector[selectedMeshIndex]->SetScale(glm::vec3(1.0f));
						meshVector[selectedMeshIndex]->SetRotation(glm::vec3(0.0f));
						meshVector[selectedMeshIndex]->SetTranslation(glm::vec3(0.0f));
					}

					if (ImGui::SliderFloat("Scale X", &meshVector[selectedMeshIndex]->scale.x, 0.1f, 10.0f)) {
						meshVector[selectedMeshIndex]->SetScale(meshVector[selectedMeshIndex]->scale);
					}

					if (ImGui::SliderFloat("Scale Y", &meshVector[selectedMeshIndex]->scale.y, 0.1f, 10.0f)) {
						meshVector[selectedMeshIndex]->SetScale(meshVector[selectedMeshIndex]->scale);
					}

					if (ImGui::SliderFloat("Scale Z", &meshVector[selectedMeshIndex]->scale.z, 0.1f, 10.0f)) {
						meshVector[selectedMeshIndex]->SetScale(meshVector[selectedMeshIndex]->scale);
					}

					// For SetRotation
					if (ImGui::SliderFloat("Rotation X", &meshVector[selectedMeshIndex]->rotation.x, -180.0f, 180.0f)) {
						meshVector[selectedMeshIndex]->SetRotation(meshVector[selectedMeshIndex]->rotation);
					}

					if (ImGui::SliderFloat("Rotation Y", &meshVector[selectedMeshIndex]->rotation.y, -180.0f, 180.0f)) {
						meshVector[selectedMeshIndex]->SetRotation(meshVector[selectedMeshIndex]->rotation);
					}

					if (ImGui::SliderFloat("Rotation Z", &meshVector[selectedMeshIndex]->rotation.z, -180.0f, 180.0f)) {
						meshVector[selectedMeshIndex]->SetRotation(meshVector[selectedMeshIndex]->rotation);
					}

					// For SetTranslation
					if (ImGui::InputFloat("Translation X", &meshVector[selectedMeshIndex]->translation.x)) {
						meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
					}

					if (ImGui::InputFloat("Translation Y", &meshVector[selectedMeshIndex]->translation.y)) {
						meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
					}

					if (ImGui::InputFloat("Translation Z", &meshVector[selectedMeshIndex]->translation.z)) {
						meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
					}
					if (ImGui::SliderFloat("Translation X2", &meshVector[selectedMeshIndex]->translation.x, -800.0f, 800.0f)) {
						meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
					}

					if (ImGui::SliderFloat("Translation Y2", &meshVector[selectedMeshIndex]->translation.y, -800.0f, 800.0f)) {
						meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
					}

					if (ImGui::SliderFloat("Translation Z2", &meshVector[selectedMeshIndex]->translation.z, -800.0f, 800.0f)) {
						meshVector[selectedMeshIndex]->SetTranslation(meshVector[selectedMeshIndex]->translation);
					}


				}
				//	ImGui::EndCombo();

			}
			ImGui::End();
		}
	if (drawIMGUI) {
		ImGui::Begin("Style Settings");

		// Background color slider
		ImGui::Text("Background Color:");
		ImGui::ColorEdit4("##bgColor", (float*)&bgColor);

		// Button color slider
		ImGui::Text("Button Color:");
		ImGui::ColorEdit4("##buttonColor", (float*)&buttonColor);

		// Apply button
		if (ImGui::Button("Apply")) {
			SetImGuiStyleColors(bgColor, buttonColor);
		}

		ImGui::End();
	}

	if (drawIMGUI) {

		ImGui::Begin("Render Shapes");
		ImGui::Checkbox("Follow Character", &character->characterActive);
		if (character->characterActive)
		{
			lMouseClicked = true;
			ImGui::Text("TFGH to control character (as WASD)");
		}
		//else { lMouseClicked = false; }
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
	// Create ImGui window
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
	glViewport(0, 0, window_width, window_height);
	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete! Error code: " << framebufferStatus << std::endl;
		// Handle the error as needed
	}

	//create screenshot im gui and tidy up after
	if (boolShowGLErrors) {
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "\nopenGL Error at: Bind framebuffer at gameloop";
		}
	}
	if (drawIMGUI) {
		ImGui::Begin("Cubes");
		ImGui::Checkbox("View Physics Engine", &boolRigidBody);
		if (ImGui::Button("Screenshot")) {
			GLubyte* pixels = new GLubyte[window_width * window_height * 4]; // 4 channels (RGBA)
			if (boolShowGLErrors) {	
				GLenum error;
				while ((error = glGetError()) != GL_NO_ERROR) {
					std::cout << "\nopenGL Error at: BEFORE pixels for saving screenshot\NError creating pixels GLubyte" << error;
				}
			}
			std::cout << "\nScreenshot parameters at time of taking: " << window_width << ", " << window_height << std::endl;
			// Read the pixel data from the framebuffer
			glReadPixels(0, 0, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			if (boolShowGLErrors) {
				GLenum error;
				while ((error = glGetError()) != GL_NO_ERROR) {
					std::cout << "\nopenGL Error at: read pixels for saving screenshot" << error;
				}
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
						//delete rigidBody->getCollisionShape();
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

			std::mt19937 rng(std::time(nullptr));
			glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
			btBoxShape* sharedBoxShape = new btBoxShape(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));
			sharedBoxShape->setMargin(0.1f);
			for (int i = 0; i < cubesToGenerate; i++) {

				//	btBoxShape* boxShapeInstance = new btBoxShape(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));
					//boxShapeInstance->setMargin(0.1f);

					// Define random distribution for positions and velocities
					//std::uniform_real_distribution<float> positionDist(minPos, maxPos); // Define minPos and maxPos as desired
				std::uniform_real_distribution<float> velocityDist(0.1f, 5.0f); // Define minVel and maxVel as desired

				float xVel = velocityDist(rng);
				float yVel = velocityDist(rng);
				float zVel = velocityDist(rng);


				glm::vec3 instancePosition;
				instancePosition.x = glm::linearRand(-300.0f, 347.0f);
				instancePosition.y = glm::linearRand(55.0f, 156.0f);
				instancePosition.z = glm::linearRand(-300.0f, 347.0f);

				//	instancePosition.y = glm::linearRand((1.0f), 34.0f);
				glm::vec3 instanceScale;
				instanceScale.x = glm::linearRand(0.1f, 1.0f);
				instanceScale.y = glm::linearRand(0.1f, 1.0f);
				instanceScale.z = glm::linearRand(0.1f, 1.0f);

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
				sharedBoxShape->calculateLocalInertia(1.0f, localInertia); // 1.0f is the mass

				btRigidBody::btRigidBodyConstructionInfo rbInfo(2.1f, motionState, sharedBoxShape, localInertia);
				btRigidBody* rigidBody = new btRigidBody(rbInfo);

				//rigidBody->setLinearVelocity(btVector3(xVel, yVel, zVel));
				btVector3 angularVelocity(0.0f, 0.0f, 0.0f); // Adjust these values as desired
				rigidBody->setAngularVelocity(angularVelocity);
				// Add rigid body to the dynamics world
				dynamicsWorldPtr->addRigidBody(rigidBody);
				cube.rigidBody = rigidBody;
				rigidBodies.push_back(rigidBody);
				cubesSSBOVector.push_back(cube);


			}
			// add the body to the dynamics world
			collisionShapes.push_back(sharedBoxShape);

			// Update the SSBO with the new data
			cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());

		}
		ImGui::End();
	}


	if (drawIMGUI) {
		ImGui::Begin("Heightmap Editor");//ImGui::Begin("Heightmap Editor", &showUI);

		// Create a button to generate the heightmap
		  // Iterate through the normals vector and display them in sets of 3
		for (size_t i = 0; i + 2 < terrain.Normals.size() && i < 16; i += 3) {
			ImGui::Text("Set %zu", i / 3); // Display a label for the set

			// Display each normal in the set
			for (int j = 0; j < 3; ++j) {
				ImGui::Text("Normal %d: (%.4f, %.4f, %.4f)", j + 1,
					terrain.Normals[i + j].x, terrain.Normals[i + j].y, terrain.Normals[i + j].z);
			}

			ImGui::Separator(); // Add a separator between sets
		}
		ImGui::Checkbox("Draw HeightMap Tools - Slows down rendering for large maps", &boolDrawHeightMap);

		if (boolDrawHeightMap) {
			// Button to export the heightmap to a raw file
			ImGui::Checkbox("Show Terrain", &terrain.boolDrawTerrainAlways);
			ImGui::Checkbox("Show Sky", &terrain.renderSkyBox);
			ImGui::Checkbox("Show Grid", &boolShowGrid);
			ImGui::Checkbox("Show Water", &boolShowWater);
			if (ImGui::Button("Export to Raw")) {
				terrain.riverPath.clear();
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
				GLuint gridsize = glGetUniformLocation(*terrain.shaderPtr, "gridSize");
				glProgramUniform1i(*terrain.shaderPtr, gridsize, terrain.size);


				if (terrain.terrainMeshRigidBody != nullptr) {
					//btCollisionShape* collisionShape = terrain.getTerrainCollionShape();
					dynamicsWorldPtr->removeCollisionObject(terrain.getTerrainMesh());
					terrain.terrainMeshRigidBody = nullptr;
					//delete collisionShape;
				}
				terrain.createTerrainMesh();

				dynamicsWorldPtr->updateAabbs();

			}
			if (ImGui::Button("Save Terrain")) {
				terrain.saveTerrainData();

			}
			if (ImGui::Button("Load  Terrain")) {
				terrain.loadTerrainData();
				glBindVertexArray(terrain.VAO);


				glBindBuffer(GL_ARRAY_BUFFER, terrain.VBO);
				glBufferData(GL_ARRAY_BUFFER, terrain.vertices.size() * sizeof(GLfloat), terrain.vertices.data(), GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain.EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrain.indices.size() * sizeof(GLuint), terrain.indices.data(), GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				// Specify vertex attribute pointers
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

				

				glGenBuffers(1, &terrain.normalBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, terrain.normalBuffer);
				glBufferData(GL_ARRAY_BUFFER, terrain.Normals.size() * sizeof(glm::vec3), terrain.Normals.data(), GL_STATIC_DRAW);


				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

			}


			if (ImGui::SliderInt("Number of Octaves", &terrain.numOctaves, 1, 20)) {
				// Recalculate octaves if the number changes
				// You can also adjust their properties here
			}
			if (ImGui::SliderFloat("Persistance", &terrain.persistance, 0.5, 5)) {
				// Recalculate octaves if the number changes
				// You can also adjust their properties here
			}
			if (ImGui::SliderFloat("Lacunarity", &terrain.lacunarity, 0.3, 5)) {
				// Recalculate octaves if the number changes
				// You can also adjust their properties here
			}
			if (ImGui::SliderFloat("Frequency", &terrain.frequency, 0.01f, 100.0f))
			{
				terrain.generateHeightMap();
			}
			
			if (ImGui::SliderFloat("Amplitude", &terrain.amplitude, 0.01f, 100.0f))
			{
				terrain.generateHeightMap();
			}
			if (ImGui::SliderInt("Size", &terrain.size, 128.f, 2048.0f)) {
				terrain.heightmapData.size = terrain.size;

				//	terrain.generateHeightMap();
			}

			if (ImGui::Button("Medium")) {
				terrain.heightmapData.size = 512;
				terrain.size = 512;
				terrain.amplitude = 50;
				terrain.frequency = 25;
			}
			if (ImGui::Button("Large")) {
				terrain.heightmapData.size = 768;
				terrain.size = 768;
				terrain.frequency = 25;
				terrain.amplitude = 50;
			}

			if (ImGui::Button("Medium Bumpy")) {
				terrain.heightmapData.size = 512;
				terrain.size = 512;
				terrain.amplitude = 75;
				terrain.frequency = 50;
			}
			if (ImGui::Button("Large Bumpy")) {
				terrain.size = 768;
				terrain.heightmapData.size = 768;
				terrain.amplitude = 75;
				terrain.frequency = 50;
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
	}

		//if (ImGui::Button("Display Physics Data" ))
		if (drawIMGUI) {
			ImGui::Begin("Physics Data");

			for (int i = 0; i < dynamicsWorldPtr->getNumCollisionObjects(); i++) {
				btCollisionObject* obj = dynamicsWorldPtr->getCollisionObjectArray()[i];

				// Check if it's a rigid body
				btRigidBody* rigidBody = btRigidBody::upcast(obj);
				if (rigidBody) {			//	// Print relevant data about the rigid body
					btCollisionShape* shape = rigidBody->getCollisionShape();
					btVector3 position = rigidBody->getCenterOfMassPosition();
					btVector3 velocity = rigidBody->getLinearVelocity();
					btVector3 angularVelocity = rigidBody->getAngularVelocity();

					//	// Get the size of the collision shape (assuming it's a box shape)
					btVector3 halfExtents;

					//	if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE) {
					//		btBoxShape* boxShape = static_cast<btBoxShape*>(shape);
					//		halfExtents = boxShape->getHalfExtentsWithMargin();
					//	}

					//	else if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
					//		ImGui::Text("Rigid Body Sphere");
					//	}
					//	else {
					//		// Handle other shape types here if needed
					//		ImGui::Text("Unknown Collision Shape Type");

					//		continue; // Skip to the next collision object
					//	}

					//	btVector3 size = halfExtents * 2.0;

					//	// Display data in ImGui window
					//	ImGui::Text("Rigid Body %d", i);
					//	//if (ImGui::Button(("Look at Shape " + std::to_string(i)).c_str())) {
					//	//	// When the button is clicked, set the camera's view matrix to look at the shape
					//	//	// You'll need to adjust this part based on your camera setup
					//	//	//glm::vec3 cameraPosition = /* Set your camera position here */;
					//	//	camera.mPitch = 0.0f;  // Reset pitch to 0 degrees
					//	//	camera.mYaw = -90.0f;
					//	//	camera.update();
					//	//	view = glm::lookAt(camera.mPosition, glm::vec3(position.x(), position.y(), (position.z() + 3.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
					//	//	camera.mPitch = 0.0f;  // Reset pitch to 0 degrees
					//	//	camera.mYaw = -90.0f;
					//	//	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

					//	//	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
					//	//	camera.mViewMatrix = view;
					//	//	camera.mPosition.x = position.x();
					//	//	camera.mPosition.y = position.y();
					//	//	camera.mPosition.z = (position.z() + 3.0f);
					//	//}
					glm::mat4 viewMatrix = glm::lookAt(camera.mPosition, glm::vec3(position.x(), position.y(), position.z() + 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));


					//	//DEBUG view matrices data for all cubes in an imgui window 'physics data' (heavy on cpu use due to imgui text)
					//	/*for (int row = 0; row < 4; ++row) {
					//		ImGui::Text("%.3f, %.3f, %.3f, %.3f", viewMatrix[row][0], viewMatrix[row][1], viewMatrix[row][2], viewMatrix[row][3]);
					//	}*/
					glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
					glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

					//	ImGui::Text("Position: (%f, %f, %f)", (std::numeric_limits<double>::max_digits10, position.x()),
					//		(std::numeric_limits<double>::max_digits10, position.y()),
					//		(std::numeric_limits<double>::max_digits10, position.z()));

					//	ImGui::Text("Size: (%f, %f, %f)", size.x(), size.y(), size.z());
					//	ImGui::Text("Linear Velocity: (%f, %f, %f)", velocity.x(), velocity.y(), velocity.z());
					//	ImGui::Text("Angular Velocity: (%f, %f, %f)", angularVelocity.x(), angularVelocity.y(), angularVelocity.z());
					//	ImGui::Text(" ");
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
	camera.character = character;
	 customVertexShaderCode.reserve(10000);
	 customFragmentShaderCode.reserve(10000);
	 stringMeshPathOverride.reserve(10000);
	 meshNameBufferInstanced.reserve(10000);

	std::string customFragmentShaderCode;
	//setup matrices
	
	bindingIndexCount = 3;
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

void updateLightingData(std::vector<World::Lights>& sceneLights, SSBO lightingSSBO, GLuint shader)
{
	 int arraySizeLocation = glGetUniformLocation(shader, "arraySize");
    glUniform1i(arraySizeLocation, sceneLights.size());

	std::vector<World::LightGPUData> gpuLightsData(sceneLights.size());
	for (size_t i = 0; i < sceneLights.size(); ++i) {
		gpuLightsData[i] = sceneLights[i].gpuData;  // Copy only the GPU data
	}

	lightingSSBO.Bind();
	lightingSSBO.updateData(gpuLightsData.data(), sizeof(sceneLights[0]) * sceneLights.size());
}


void SetImGuiStyleColors(ImVec4 bgColor, ImVec4 buttonColor) {
	ImGuiStyle& style = ImGui::GetStyle();

	// Set background color
	style.Colors[ImGuiCol_WindowBg] = bgColor;

	// Set button color
	style.Colors[ImGuiCol_Button] = buttonColor;
}

void generateSSBOInstance(int count, std::string name) {
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	btBoxShape* sharedBoxShape = new btBoxShape(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));
	sharedBoxShape->setMargin(0.1f);
	std::vector<World::cubeInstance>instanceDataVector;
	for (int i = 0; i < count; i++)
	{
		glm::vec3 instancePosition;
		instancePosition.x = glm::linearRand(-150.0f, 157.0f);
		instancePosition.y = glm::linearRand(-155.0f, 156.0f);
		instancePosition.z = glm::linearRand(-150.0f, 157.0f);

		//	instancePosition.y = glm::linearRand((1.0f), 34.0f);
		glm::vec3 instanceScale;
		float randomScale = glm::linearRand(0.01f, 0.5f);
		instanceScale.x = randomScale;
		instanceScale.y = randomScale;
		instanceScale.z = randomScale;

		glm::mat4 modelMatrix(1.0f); // Initialize the model matrix as an identity matrix

		// Apply scale
		modelMatrix = glm::scale(modelMatrix, instanceScale);

		// Apply translation
		modelMatrix = glm::translate(modelMatrix, instancePosition);
		float angleX = glm::linearRand(0.0f, 2.0f * glm::pi<float>());
		float angleY = glm::linearRand(0.0f, 2.0f * glm::pi<float>());
		float angleZ = glm::linearRand(0.0f, 2.0f * glm::pi<float>());

		modelMatrix = glm::rotate(modelMatrix, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, angleZ, glm::vec3(0.0f, 0.0f, 1.0f));

		// No rotation is applied in this version

		World::cubeInstance instance;
		instance.modelMatrix = modelMatrix;
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
		sharedBoxShape->calculateLocalInertia(1.0f, localInertia); // 1.0f is the mass

		btRigidBody::btRigidBodyConstructionInfo rbInfo(2.1f, motionState, sharedBoxShape, localInertia);
		btRigidBody* rigidBody = new btRigidBody(rbInfo);

		//rigidBody->setLinearVelocity(btVector3(xVel, yVel, zVel));
		btVector3 angularVelocity(0.0f, 0.0f, 0.0f); // Adjust these values as desired
		rigidBody->setAngularVelocity(angularVelocity);


		// Add rigid body to the dynamics world
		//dynamicsWorldPtr->addRigidBody(rigidBody);
		instance.rigidBody = rigidBody;
		//rigidBodies.push_back(rigidBody);
		instanceDataVector.push_back(instance);

	}
	//cubeSSBO.updateData(cubesSSBOVector.data(), sizeof(cubesSSBOVector[0]) * cubesSSBOVector.size());
	SSBOVector[SSBOVector.size() - 1]->Bind();

	//DOCS::Originally in the parent function the SSBO is created with a larger vector, but it is reset here and so should be optomised already.
	//DOCS::The SSBO function below only takes the one instance and not all previously loaded meshes
	SSBOVector[SSBOVector.size() - 1]->updateData(instanceDataVector.data(), sizeof(instanceDataVector[0]) * instanceDataVector.size());
	SSBOVector[SSBOVector.size() - 1]->setDataVector(instanceDataVector);
	SSBO* key = SSBOVector.back();
	key->SSBOName = name;
	// Check if the key is already present in the map

		// If the key is not present, insert a new key-value pair
	mapSSBOMeshInstanceVector.insert(std::make_pair(*key, instanceDataVector));

	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after updating SSBO: " << error << std::endl;
	}
}




void generateSSBOInstanceToY(const std::vector<float>& terrainHeights, int terrainSize, int numberOfMeshes, float minY, float maxY, std::string name, float scalemin, float scalemax) {
	// Terrain vector is: terrain.heightmapData.heights

	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	btBoxShape* sharedBoxShape = new btBoxShape(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));
	sharedBoxShape->setMargin(0.1f);
	std::vector<World::cubeInstance> instanceDataVector;

	for (int i = 0; i < numberOfMeshes; ++i) {
		glm::vec3 instancePosition;

		// Generate random positions until a valid one is found
		float xPos = static_cast<float>(rand() % terrainSize);
		float zPos = static_cast<float>(rand() % terrainSize);

		// Ensure that the calculated positions are within the terrain bounds
		/*xPos = 0;
		zPos = 0;*/

		// Extract the Y position from the terrain heights
		float yPos = terrainHeights[static_cast<int>(xPos) + static_cast<int>(zPos) * terrainSize];
		//Pos -= 2.0;
		// Apply any offset or adjustment to yPos for realism
		// For example, sink the meshes into the ground a little
		// yPos -= /* your desired offset */;

		// Now, use the yPos for the instance position
		instancePosition = glm::vec3(xPos, yPos, zPos);

		glm::vec3 instanceScale;
		float randomScale = glm::linearRand(scalemin, scalemax);
		instanceScale.x = randomScale;
		instanceScale.y = randomScale;
		instanceScale.z = randomScale;

		// Initialize the model matrix as an identity matrix
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), instancePosition);

		// Apply scale
		modelMatrix = glm::scale(modelMatrix, instanceScale);

		// ... rest of the code remains unchanged ...

		World::cubeInstance instance;
		instance.modelMatrix = modelMatrix;
					// Set initial position and orientation
					btTransform startTransform;
					startTransform.setIdentity();
					startTransform.setOrigin(btVector3(instancePosition.x, instancePosition.y, instancePosition.z));

					// Create a rigid body
					btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
					btVector3 localInertia(0, 0, 0);
					sharedBoxShape->calculateLocalInertia(1.0f, localInertia); // 1.0f is the mass

					btRigidBody::btRigidBodyConstructionInfo rbInfo(2.1f, motionState, sharedBoxShape, localInertia);
					btRigidBody* rigidBody = new btRigidBody(rbInfo);

					btVector3 angularVelocity(0.0f, 0.0f, 0.0f); // Adjust these values as desired
					rigidBody->setAngularVelocity(angularVelocity);

					instance.rigidBody = rigidBody;
					instanceDataVector.push_back(instance);

					// Break out of the loop once a valid position is found
					
				
			
		}
	SSBOVector[SSBOVector.size() - 1]->Bind();

	//DOCS::Originally in the parent function the SSBO is created with a larger vector, but it is reset here and so should be optomised already.
	//DOCS::The SSBO function below only takes the one instance and not all previously loaded meshes
	SSBOVector[SSBOVector.size() - 1]->updateData(instanceDataVector.data(), sizeof(instanceDataVector[0]) * instanceDataVector.size());
	SSBOVector[SSBOVector.size() - 1]->setDataVector(instanceDataVector);
	SSBO* key = SSBOVector.back();
	key->SSBOName = name;
	// Check if the key is already present in the map

		// If the key is not present, insert a new key-value pair
	mapSSBOMeshInstanceVector.insert(std::make_pair(*key, instanceDataVector));

		// Cleanup
		
	

	// Rest of your code...
}

std::vector<glm::mat4> generateModelMatricesFromTerrain(const std::vector<float>& terrainHeights, int terrainSize, int numberOfMeshes, float minY, float maxY) {
	std::vector<glm::mat4> modelMatrices;

	// Generate model matrices for each mesh
	for (int i = 0; i < numberOfMeshes; ++i) {
		// Calculate X and Z positions based on terrain dimensions
		float xPos = static_cast<float>(rand() % terrainSize);  // Example: Random X position within terrain size
		float zPos = static_cast<float>(rand() % terrainSize);  // Example: Random Z position within terrain size

		// Map the 2D terrain coordinates to the 1D array index
		int terrainIndex = static_cast<int>(xPos) + static_cast<int>(zPos) * terrainSize;

		// Ensure that the calculated positions are within the terrain bounds
		xPos = std::max(0.0f, std::min(static_cast<float>(terrainSize - 1), xPos));
		zPos = std::max(0.0f, std::min(static_cast<float>(terrainSize - 1), zPos));

		// Extract the Y position from the terrain heights
		float yPos = terrainHeights[terrainIndex];

		// Apply any offset or adjustment to yPos for realism
		// For example, sink the meshes into the ground a little
		
		//yPos -= /* your desired offset */;

		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, zPos));

		// Add the model matrix to the vector
		modelMatrices.push_back(modelMatrix);
	}

	return modelMatrices;
}
void drawFBOToScreen(const std::string& fboName, GLuint textureID, int width, int height) {
    GLenum err;

    // Check if the FBO name exists in the map
    auto it = framebuffers.find(fboName);
    if (it == framebuffers.end()) {
        std::cerr << "Error: FBO name '" << fboName << "' not found in the map!" << std::endl;
        return;
    }

    const FramebufferObject& fboObj = it->second;  // Get the framebuffer object

    // Use the shader program associated with the terrain
    glUseProgram(*terrain.shaderPtr);

    // Ensure width and height are valid
    if (width <= 0 || height <= 0) {
        std::cerr << "Error: Invalid width or height for FBO (" << width << ", " << height << ")" << std::endl;
        return;
    }

    // Bind the custom framebuffer (FBO)
    glBindFramebuffer(GL_FRAMEBUFFER, fboObj.fbo);  // Use the FBO ID from the map
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the framebuffer

    // Bind the shader and set the uniform for terrain picking mode
    GLint editModeLocation = glGetUniformLocation(*terrain.shaderPtr, "terrainEditMode");
    GLint viewLoc = glGetUniformLocation(*terrain.shaderPtr, "view");

    // Check if custom view is enabled, and set the view matrix appropriately
    if (fboObj.customViewEnabled) {
        // Use the custom view matrix stored in the FBO object
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(fboObj.viewMatrix));
    } else {
        // Use the default camera view matrix
        glm::mat4 defaultViewMatrix = camera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(defaultViewMatrix));
    }

    // Check the terrain picking mode and render the terrain accordingly
    if (terrainPickingSwitch == 1) {
        terrainPickingSwitch = 0;  // Disable picking mode

        // Set the uniform for terrain picking mode
        glUniform1i(editModeLocation, terrainPickingSwitch);

        // Bind the texture and render the terrain in normal mode
        glBindTexture(GL_TEXTURE_2D, textureID);
        terrain.render();  // Render the terrain

        // Restore picking mode
        terrainPickingSwitch = 1;
        glUniform1i(editModeLocation, terrainPickingSwitch);  // Update the uniform back to picking mode
    } else {
        // Normal render mode
        glBindTexture(GL_TEXTURE_2D, textureID);
        terrain.render();  // Render the terrain
    }

    // Unbind the texture after rendering
    glBindTexture(GL_TEXTURE_2D, 0);

    // Check for any errors
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error after terrain rendering: " << err << std::endl;
    }

    // Render the framebuffer in an ImGui window with a unique title
    std::string windowName = "Framebuffer: " + fboObj.name;
    ImGui::Begin(windowName.c_str());  // Create a unique window for each FBO
    ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    // Check for any errors after ImGui rendering
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error after ImGui rendering: " << err << std::endl;
    }

    // Unbind the framebuffer after rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}





void calculateGradients(const std::vector<float>& terrain, int width, int height,
	std::vector<float>& gradientX, std::vector<float>& gradientY) {
	// Initialize vectors to store gradients
	gradientX.resize(terrain.size());
	gradientY.resize(terrain.size());

	// Calculate gradients
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// Calculate finite differences for x and y
			float dx = terrain[y * width + std::min(x + 1, width - 1)] - terrain[y * width + x];
			float dy = terrain[std::min(y + 1, height - 1) * width + x] - terrain[y * width + x];

			// Store the gradients
			gradientX[y * width + x] = dx;
			gradientY[y * width + x] = dy;
		}
	}
}


// Function to create a normal map from gradients
void createNormalMap(const std::vector<float>& gradientX, const std::vector<float>& gradientY,
	int width, int height, std::vector<glm::vec3>& normalMap) {
	// Initialize vector to store the normal map
	normalMap.resize(gradientX.size());

	// Create normal map
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// Calculate normalized normal vector
			float len = std::sqrt(1.0f + gradientX[y * width + x] * gradientX[y * width + x] +
				gradientY[y * width + x] * gradientY[y * width + x]);

			// Convert the normal vector to the [0, 1] range for storage in a texture
			normalMap[y * width + x].x = 0.5f * gradientX[y * width + x] / len + 0.5f;
			normalMap[y * width + x].y = 0.5f * gradientY[y * width + x] / len + 0.5f;
			normalMap[y * width + x].z = 0.5f;
		}
	}
}

void checkGLError(const char* functionName) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error in function " << functionName << ": " << error << std::endl;
	}
}


void debugPrintPixels(const unsigned char* pixels, int width, int height) {
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int index = (i * width + j) * 3;  // Assuming 3 channels (RGB)
			int r = pixels[index];
			int g = pixels[index + 1];
			int b = pixels[index + 2];

			// Check if the pixel is not black
			if (r != 0 || g != 0 || b != 0) {
				std::cout << "Non-black pixel found at (" << j << ", " << i << "): "
					<< "R=" << r << ", G=" << g << ", B=" << b << std::endl;
			}
		//	else std::cout << "Black pixel";
		}
	}
}

void CC(std::string location, std::string comment) {

	std::cout << "\n" << location << ": " << comment;
}


void printMatrix(glm::mat4 matrix, std::string location)
{
	std::cout << "\n\tMatrix at location:" << location << "\n";
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}


void setimGUILook() {

	ImGuiStyle& style = ImGui::GetStyle();

	// Background color
	style.Colors[ImGuiCol_WindowBg] = ImVec4(37.0f / 255.0f, 117.0f / 255.0f, 119.0f / 166.0f, 255.0f / 255.0f);

	// Button color
	style.Colors[ImGuiCol_Button] = ImVec4(218.0f / 255.0f, 063.0f / 255.0f, 113.0f / 255.0f, 255.0f / 255.0f);

	// Optionally, button hover and active color
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(218.0f / 255.0f, 183.0f / 255.0f, 33.0f / 255.0f, 255.0f / 255.0f);  // Slightly lighter for hover
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(218.0f / 255.0f, 143.0f / 255.0f, 13.0f / 255.0f, 255.0f / 255.0f);   // Darker for active button
}


void setupFramebuffer(const std::string& name, int width, int height, bool customViewEnabled) {
	FramebufferObject fbo;
	fbo.name = name;
	fbo.width = width;
	fbo.height = height;
	fbo.textureName = name + "_texture";
	fbo.customViewEnabled = customViewEnabled;
	// Generate and bind framebuffer
	glGenFramebuffers(1, &fbo.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.fbo);

	// Generate texture to hold color attachment
	glGenTextures(1, &fbo.texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, fbo.texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texColorBuffer, 0);

	// Generate renderbuffer for depth and stencil
	glGenRenderbuffers(1, &fbo.rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, fbo.rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo.rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	else
		std::cout << "Framebuffer '" << fbo.name << "' created with texture '" << fbo.textureName << "'" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
	if (customViewEnabled) {
		fbo.viewMatrix = camera.getViewMatrix();
	}
	// Store the new framebuffer in the map
	framebuffers[fbo.name] = fbo;
}

void showLoadModelWindow(bool* pOpen) {
	ImGui::Begin("Load Model", pOpen);

	// char objFilePath[256] = ""; // Buffer for the file path input
//	 std::string assimpFilename; // No initial assignment needed

//	ImGui::InputText("OBJ File Path", objFilePath, sizeof(objFilePath));

	//if (ImGui::Button("Load OBJ")) {
	//	//std::string filePath(objFilePath);
	//	//ObjImporter obj;

	//	//// Load the OBJ file and handle errors
	//	//if (obj.loadOBJ(filePath)) {
	//	//	models.push_back(obj); // Only push if the load was successful
	//	//}
	//	//else {
	//	//	std::cerr << "Failed to load OBJ file: " << filePath << std::endl;
	//	//}
	//}
	
	if (ImGui::CollapsingHeader("Load Models New"))
	{
		ImGui::Checkbox("Pause Model Renderer", &modelLoaderNew.boolDontRender);
		const char* aFilterPatterns[] = { "*.obj", "*.fbx" };

		if (ImGui::Button("Open/Load Model")) {
			FILE* lIn;
			lIn = nullptr;
			constexpr size_t bufferSize = 512; // Define a reasonable buffer size
			char filePathBuffer[bufferSize] = ""; // Buffer for safely handling filename
			char const* lTheOpenFileName = nullptr;

			std::cout << "\nUser pressed 'open/load model' in main.cpp";
			fname.clear();
			std::cout << "\nWill now attempt to open dialog with parameters (main.cpp)";

			try {
				// Attempt to open the file dialog
				lTheOpenFileName = tinyfd_openFileDialog(
					"Select Image",
					"",
					2,
					aFilterPatterns,
					NULL,
					0
				);
				
			
				// Check if the file dialog was successful
				if (lTheOpenFileName == nullptr) {
					std::cout << "File dialog failed or was cancelled.";
				}
				else { lIn = fopen(lTheOpenFileName, "r"); 
				// Copy filename to buffer safely
				strncpy(filePathBuffer, lTheOpenFileName, bufferSize - 1);
				filePathBuffer[bufferSize - 1] = '\0'; // Ensure null termination

				std::cout << "Selected file: " << filePathBuffer << std::endl;
				}
			
			}
			catch (const std::exception& e) {
				std::cerr << "Error: " << e.what() << std::endl;
			}
			catch (...) {
				std::cerr << "An unknown error occurred." << std::endl;
			}

			if (strlen(filePathBuffer) > 0 && lTheOpenFileName != nullptr) { // Proceed only if a valid filename was selected
				//fname = filePathBuffer;
				std::string assimpFilename2 = filePathBuffer;
				//logger.AddLog("Selected file: " + filePathBuffer);

				// Model cleanup logic if needed
				// modelLoaderNew.cleanup();

				for (int i = 0; i < 1; ++i) {
					const aiScene* scene = importer->ReadFile(
						assimpFilename2.c_str(),
						aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
					);  
					

					if (scene) {
						scenes.push_back(scene);
						modelLoaderNew.initFromScene(scene, assimpFilename2);

						std::string modelLoadedLog = "Model Loaded - Instance " + std::to_string(i + 1);
						logger.AddLog(modelLoadedLog, ImGuiLogger::LogType::Assimp);

						for (auto& m : modelLoaderNew.modelNewVector) {
							std::string modelNameLog = "Loaded: " + m->name + " - Instance " + std::to_string(i + 1);
							logger.AddLog(modelNameLog, ImGuiLogger::LogType::Assimp);
						}

						//s aimporter->FreeScene(); // Free after processing each model
					}
					else {
						const char* errorMsg = importer->GetErrorString();
						std::string errorLogMessage = "Error loading model: " + std::string(errorMsg);
						logger.AddLog(errorLogMessage, ImGuiLogger::LogType::Error);
					}
				}

				//fname.clear(); // Clear filename after processing
				//assimpFilename2.clear();
				//(lIn);
			}
			else {
				logger.AddLog("No file selected.", ImGuiLogger::LogType::Warning);
			}
		}

		if (ImGui::Button("Delete Models")) {
			modelLoaderNew.modelNewVector.clear();
			return;
		}
		if (ImGui::Button("Delete last")) {
			if (modelLoaderNew.modelNewVector.size() > 0) {
				modelLoaderNew.modelNewVector.pop_back();
				return;
			}
		}

		if (ImGui::ColorEdit3("- Light Colour - ", &sun.DiffuseColor.x)) {
			GLint diffuseColorLocation = glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "diffuseColor");
			glUniform3fv(diffuseColorLocation, 1, &sun.DiffuseColor.x);

		}


		if (ImGui::SliderFloat("Sun Brightness", &sun.Brightness, 0.1, 5.0f)) {

			GLint sunBrightnessLocation = glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "sunBrightness");
			glUniform1f(sunBrightnessLocation, sun.Brightness);

		}

		//if (ImGui::SliderFloat("Time of Day", &timeOfDay, 0.0f, 24.0f)) {
		//	// Update the sun's position whenever the time of day changes
		//	// You can use this callback to trigger any other time-dependent effects
		//	 sunX = glm::clamp(-std::abs(timeOfDay - 12.0f) / 6.0f + 1.0f, -1.0f, 1.0f);
		//	 sunY = glm::clamp(-std::abs(timeOfDay - 12.0f) / 6.0f + 1.0f, -1.0f, 1.0f);

		//	sunPosition = glm::vec3(sunX, -sunY, 0.0f);
		//	glUniform3fv(lightDirectionLocation, 1, &sunPosition.x);

		//}
			//timeOfDay = 18.0;
		if (sun.radiansTime >= 360.0) {
			// Switch to decreasing
			sun.movingForward = false;
		}
		else if (sun.radiansTime <= 0.0) {
			// Switch to increasing
			sun.movingForward = true;
		}

		if (sun.movingForward) {
			sun.radiansTime += 0.30;
		}
		else {
			sun.radiansTime -= 0.30;
		}
		if (ImGui::SliderFloat("Time", &sun.radiansTime, 0.0f, 360.0f)) {
			GLint sunRadiansTimeLocation = glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "radiansTime");
			glUniform1f(sunRadiansTimeLocation, sun.radiansTime);

		}
		
	}
	if (modelLoaderNew.shaderProgram != nullptr)
	{
		GLint sunRadiansTimeLocation = glGetUniformLocation(modelLoaderNew.shaderProgram->ID, "radiansTime");
		glUniform1f(sunRadiansTimeLocation, sun.radiansTime);
	}

		

	
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	if (!io.WantCaptureMouse) {
		if (yoffset > 0) {
			// Mouse scrolled up
			std::cout << "Scrolled up: " << yoffset << std::endl;
			if (terrainPickingSwitch == 1)
			{

				boolToolResized = true;
			}

			camera.mPosition += camera.mFront * camera.mMovementSpeed * 14.0f;
		}
		else if (yoffset < 0) {
			// Mouse scrolled down
			std::cout << "Scrolled down: " << xoffset << std::endl;
			if (terrainPickingSwitch == 1)
			{
				boolToolResized = true;
			}
			camera.mPosition -= camera.mFront * camera.mMovementSpeed * 14.0f;
		}

		// Assume mMovementSpeed is scaled appropriately
		//mPosition += mFront * mMovementSpeed;

		//camera.update();
		if (currentTool == Tool::TERRAIN_SCULPT) {
			if (yoffset > 0) {
				brushSize += 1.0; // Increase brush size
			}
			else if (yoffset < 0) {
				brushSize -= 1.0; // Decrease brush size
			}
		}
		//camera.mPosition += camera.mFront * camera.mMovementSpeed * 3.0f;
		camera.update();
	}
}