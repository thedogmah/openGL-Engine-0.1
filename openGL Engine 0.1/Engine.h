#pragma once
// Include this only in one source file

#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <random>
#include "globals.h"
#include <vector>
#include <algorithm>

#include "C:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\BulletDynamics\btBulletDynamicsCommon.h"


#include <c:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "c:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

void calculateGradients(const std::vector<float>& terrain, int width, int height,
	std::vector<float>& gradientX, std::vector<float>& gradientY);
void createNormalMap(const std::vector<float>& gradientX, const std::vector<float>& gradientY,
	int width, int height, std::vector<glm::vec3>& normalMap);

class Engine
{
};

struct heightmap {

	char* heightData;// height data
	int size = 256; //the height size, must be power of two*
	std::vector<float> heights;
};

class Terrain {


public:
	heightmap heightmapData;
	float heightScale = 0.0; //scaling variable
	std::fstream fstreamObj;

	//height map values;
	float frequency = 3.0f; // Adjust this to control the scale of details
	float amplitude = 12.0f;


	//Function for serialising Vectors to save engine data to file and to load up.

	void saveVectorToFile(const std::vector <GLfloat>& vec, const std::string& filename) {
	
		std::ofstream file(filename);
		if (file.is_open()) {
			for (const auto& value : vec)
			{
				file << value << " ";
			
			}
			file.close();
		
		}

		else {
		
			std::cout << "Unabe to open file for saving Vertices of terrain (Engine.H)";
		}

	}
	void saveVec3ToFile(const std::vector<glm::vec3>& vec, const std::string& filename){

		std::ofstream file(filename);
		if (file.is_open()) {
		
			for (const auto& value : vec) {
			
				file << value.x << " " << value.y << " " << value.z << " ";
			}
			file.close();
		}
	
		
		else {

			std::cout << "Unabe to open file for saving Normals of terrain (Engine.H)";
		} 
	
	}
	void saveIndicesToFile(const std::vector<GLuint>& vec, const std::string& filename)
		{
		std::ofstream file(filename);
		if (file.is_open()) {

			for (const auto& value : vec)
			{
				file << value << " ";

			}
			file.close();
		}
		else
		{
			std::cout << "Unable to open indices file within engine.h";
			logger.AddLog("Unable to Save Terrain to File", ImGuiLogger::LogType::Warning);
		
		}
	}
	
	void loadVectorFromFile(std::vector<GLfloat>& vec, const std::string& filename) {
	
		std::ifstream file(filename);
		if (file.is_open()) {
			vec.clear();
			GLfloat value;

			while (file >> value)

			{
				vec.push_back(value);
			}
		
			file.close();
		}
		else {

			std::cout << "Unable to open file for loading terrain";

		}
	}


	void loadVec3FromFile(std::vector<glm::vec3>& vec, const std::string& filename) {
		std::ifstream file(filename);
		if (file.is_open()) {
			vec.clear();
			GLfloat x, y, z;
		
			while (file >> x >> y >> z) {
				vec.emplace_back(x, y, z);
			}
			file.close();
		}
		else {
			std::cout << "Unable to open file for loading Normals of terrain (Engine.H)";
					
		}
	}
	void loadIndicesFromFile(std::vector<GLuint>& vec, const std::string& filename) {
		std::ifstream file(filename);
		if (file.is_open()) {
			vec.clear();
			GLuint value;
			while (file >> value) {
				vec.push_back(value);
			}
			file.close();
		}
		else {
			std::cout << "Unable to open indices file within engine.h";
			logger.AddLog("Unable to Load Terrain from File", ImGuiLogger::LogType::Warning);
		}
	}

	Terrain();
	unsigned int* shaderPtr = nullptr;
	//Do I need destructor for parent class?
	int size =256; //must be a power of two
	struct Vertex {
		glm::vec3 position; // x, y, z
		glm::vec3 color;    // r, g, b (optional)
	};
	void render() ;
	void initalise();
	bool loadHeightMap(char* filename, int size);
	bool createTerrainMesh();
	bool saveHeightMap(char* filename);
	bool unloadHeightMap();
	void generateHeightMap();
	std::pair<int, int> indexToCoordinates(int index);//function converts 1d index to 2d coordinates
	int coordinatesToIndex(int i, int j); //converts 2d coordinates to 1d index;
	int findHighestPoint(const std::vector<GLfloat>& vertices);
	std::mt19937 rng;  // Mersenne Twister PRNG
	int numOctaves = 4; // for heightmap
	void createUVs();
	std::vector<GLfloat> GenerateFractalTerrain(std::vector<GLfloat>& Vertices,
	int iIterations,
	float iHeight,
	float iMinDelta,
	float iMaxDelta);
	void terrainEditUI(GLFWwindow* window);
	void initPickingBuffer();
	void terrainRenderToFBO();
	std::vector<GLfloat> terrainPickedLocationsVector;
	std::vector<GLfloat> currentTerrainClickedRGB;
	int rgbSelected[4];//For terrain fbo picking after retrieving co ords from other array
	int rgbSelectedTerrain[4];//for terrain picking colour
	//extern bool terrainLMouseClicked = false;
	void pickTerrain(GLFWwindow* window);
	void toolsCircleBrush(std::vector<float>& vertices, int X, int Z, int size, float radius, float newHeight);
	void toolsSquareBrush(std::vector<float>& vertices, int X, int Z, int size, float squaresize, float newHeight);
	void toolsFlattenBrush(std::vector<float>& vertices, int X, int Z, int size, float radius);

	void terrainBackup(); //function called to add 'vertices' to a container of historical vertices. (the terrain is saved in the collection to revert back to but 
	//must also consider if other objects are saved alongside it.
	
	void drawTerrainHistory();
	enum BrushType {
		POINT_BRUSH,
		CIRCLE_BRUSH,
		SQUARE_BRUSH,
		H_, LINE_BRUSH,
		V_LINE_BRUSH,
		FLATTEN_BRUSH		//Add more brushes as needed
	} currentBrushType;

	float brushRadius = 15;

	int brushImpact = 6;
	bool boolRelativeHeight = true;
	bool boolTerrainToolSwitch = false;//Use separate INT for shader if statement, since shaders dont use BOOLEANS
	btRigidBody* getTerrainRigidBody();
	btCollisionShape* getTerrainCollionShape();
	btRigidBody* getTerrainMesh();
	glm::vec3 calculateSurfaceNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	float lacunarity = 2.0;
	float persistance = 2.0;
	//terrain customisations functions
	void fractalTerrain();
	void voxelateTerrain();
	void firSmoothTerrain();
	float mountain_scaling = 1.5f;  // Adjust to control mountain height
	
	//Terrain Buffer Data
	struct TerrainInfo {
	int isWater =0;
	int isMountain=0;
	int isForest=0;
	int isDesert=0;
};
	//River members
	void generateRiver(std::vector<GLfloat>& vertices, int startPointIndex, int finalPointIndex, int numMidPoints);
	float riverBedValue = 0.0f; // You can set an initial value here
	std::random_device rd;//FOR GENERATING RIVER VARIABLES / PATHS
	
	std::vector<int> riverPath;

	void mountainsTerrain();
	void renderTextureLoader();
	GLuint loadTexture(const char* path);
	std::vector<GLuint> textureIDs;
	std::vector<std::string> textureNames;
	float repeatFactor = 8.0f;//Higher number is more repetition sooner in texturing (create uv function)
	float terrainBottom = 0, terrainTop = 0;
	int selectedTextureIndex = -1;
	char fileNameBuffer[256] = ""; // Assuming a reasonable file name length// for loadig texture filename. So it stays in scope
	bool boolTextureLoadSuccess = false; //Bool is true when most recent attempt to load texture was successful, and so the new texture name is added to the imgui explorer
	std::vector<std::string> uniformNames = {
	"txGrass",
	"txHighGrass",
	"txRock",
	"txHighRock",
	"txPeak"
	};
	GLuint waterTextureID;
	//terrain displacemtnuniforms
		// Control for heightOffset
	float slopeThreshold = 0.1;
	 float theightScale = 1.0f;  // Initial value
	 float heightOffset = 0.0f;  // Initial value
	float smoothStep;
	inline void setHeightScale(float fScale) {
		heightScale = fScale;
	}

	inline void setHeightAtPoint(unsigned char height, int x, int z) {
	//Set the true height value at the given point
		heightmapData.heightData[(z * size) + x] = height;
	}

	//A function to get the true height value (0-255) at a point
	inline unsigned char getTrueHeightAtPoint(int x, int z) {
		return (heightmapData.heights[(z*size)+x]);
	
	}
	//Retrieve the scaled height at a given point
	inline float getScaledHeightAtPoint(int x, int z) {
		return (heightmapData.heights[(z * size) + x]) * heightScale;
	
	}


	//GL members
	//std::vector<Vertex> vertices;

	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> uvs;
	GLuint VAO, VBO, EBO, normalBuffer, colorBuffer, uvVBO, terrainVBO, verticesIDVBO, TerrainClickedRGB_VBO;
	GLuint terrainPickFBO, terrainPickTexture, terrainPickDepthBuffer, terrainPickPickingBuffer; //Frame buffer for color / mouse picking / or ray cast solution.
 
	GLuint eng_fbo, eng_texColorBuffer, eng_rbo; //frame buffer variables etc for rendering the world to an iimgui window


	//Switch this to a 1 to tell the shaders we're in terrain edit mode.
	GLenum error;
	
	// Reserve space for vertices (assuming m_iSize is the size of your heightmap)

	//gradients for terrain for normal mapping calculations
	std::vector<float> gradientX, gradientY;
	//norrmal map

	float drawDistance = 100.0f;
	bool useNormalMap = 0;
	bool useDetailMap = 0;
	GLuint normalMapTexture;
	GLuint detailMapTexture;
	GLuint mudMapTexture, mudHeight, mudNormals;
	std::vector<glm::vec3> normalMap;
	//Shader variables
	glm::vec3  rockyColor, ambientColor, diffuseColor, specularColor;
	glm::vec3 waterColor = glm::vec3(0.0, 0.2, 0.8);
	glm::vec3 snowColor = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3  grassColor = glm::vec3(0.1, 0.75, 0.1);
	glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	float waterThreshold, grassThreshold, rockyThreshold, snowThreshold, peakThreshold;
	float waterStopThreshold = 0.1f; // Adjust the default values as needed
	float grassStopThreshold = 0.3f;
	float rockyStopThreshold = 0.6f;
	float snowStopThreshold = 0.9f;
	float currentTime = 0.0f;
	float shininess = 0.3;
	float sunBrightness = 1.0;
	float timeOfDay = 12.0f;
	float radiansTime = 90;// time of day (radians)

	float sunX = glm::clamp(-std::abs(timeOfDay - 12.0f) / 6.0f + 1.0f, -1.0f, 1.0f);
	float sunY = glm::clamp(-std::abs(timeOfDay - 12.0f) / 6.0f + 1.0f, -1.0f, 1.0f);
	glm::vec3 sunPosition;
	bool ready = false; //switch to ready when ready for calling render function. This prevents the game loop rendering
	//the terrain before it exists.
	//physics
	btTriangleMesh* terrainMesh = nullptr;
	btHeightfieldTerrainShape* terrainShapePtr = nullptr;
	btRigidBody* terrainRigidBody = nullptr;
	btRigidBody* terrainMeshRigidBody = nullptr;
	btDefaultMotionState* terrainMotionState = nullptr;
	bool applyModelTransform = false;
	bool boolFractalTerrain = false;
	bool boolVoxelateTerrain = false;
	bool boolFIRErosion = false;
	bool boolTrimEdges = false;
	//terrain members
	
	std::vector<GLfloat> vertices; //vertices of the terrain itself
	std::vector<GLfloat> verticesID;
	std::vector<GLuint> indices; //for vertices of terrain
	int currentHistoryIndex = -1;  // -1 means active terrain

	void saveTerrainData()
	{

		saveVectorToFile(vertices, "vertices.txt");//save vertices to file
		saveVec3ToFile(Normals, "normals.txt");
		saveIndicesToFile(indices, "indices.txt");
	}

	void loadTerrainData()
	{

		loadVectorFromFile(vertices, "vertices.txt");//save vertices to file
		loadVec3FromFile(Normals, "normals.txt");
		loadIndicesFromFile(indices, "indices.txt");
	}


	std::vector<std::vector<GLfloat>> verticesHistory;
	
	std::vector<TerrainInfo> terrainInfoVector;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	 float translationX = 0.0f;
	 float translationY = 0.0f;
	 float translationZ = 0.0f;
	 float yScale = 1.0f; // Initialize the Y-axis scale factor
	 
	 float previousYScale = 1.0f;//scaling tterrain via UI. Necessary for sliding.
	 btDiscreteDynamicsWorld* dynamicsWorldUniversalPtr = NULL;
	 bool init = false;
	 bool boolDrawTerrainAlways = true; //created a UI tool to not draw terrain for debugging
	 bool renderSkyBox = true; 
	

	 //switches for fog and extra weather effects.
	 bool enableFog = false;
	 bool enableFog2 = false;
	 bool enableHighWinds = false;
	 float fogStart = 30.0;
	 float fogEnd = 50.0;
	 
	 glm::vec3 fogColor;


	 int numIterations = 3;
	 float kernelPoint = 9.0;

	 int terrainIterations = 5;
	 float  terrainHeight = 5;
	 float terrainMinDelta = 1;
		 float terrainMaxDelta = 40;

		 struct Voxel {
			 bool isOccupied; // Whether the voxel is part of the terrain or not
			 // You can add more properties here if needed
		 };

};


void drawShaderManager();


GLuint loadCustomShader(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
