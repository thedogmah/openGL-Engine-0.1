#pragma once
// Include this only in one source file
#include <stb/stb_image.h>


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

#include <vector>

#include "btBulletDynamicsCommon.h"

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
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
public:
	
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
	std::mt19937 rng;  // Mersenne Twister PRNG
	int numOctaves = 5; // for heightmap
	void createUVs();
	std::vector<GLfloat> GenerateFractalTerrain(std::vector<GLfloat>& Vertices,
	int iIterations,
	float iHeight,
	float iMinDelta,
	float iMaxDelta);
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
	float mountain_scaling = 4.0f;  // Adjust to control mountain height

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
		return (heightmapData.heightData[(z*size)+x]);
	
	}
	//Retrieve the scaled height at a given point
	inline float getScaledHeightAtPoint(int x, int z) {
		return (heightmapData.heightData[(z * size) + x]) * heightScale;
	
	}


	//GL members
	//std::vector<Vertex> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	GLuint VAO, VBO, EBO, normalBuffer, colorBuffer, uvVBO;
	GLenum error;
	// Reserve space for vertices (assuming m_iSize is the size of your heightmap)


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
	float sunX = glm::clamp(-std::abs(timeOfDay - 12.0f) / 6.0f + 1.0f, -1.0f, 1.0f);
	float sunY = glm::clamp(-std::abs(timeOfDay - 12.0f) / 6.0f + 1.0f, -1.0f, 1.0f);
	glm::vec3 sunPosition;
	bool ready = false; //switch to ready when ready for calling render function. This prevents the game loop rendering
	//the terrain before it exists.
	//physics
	btTriangleMesh* terrainMesh = nullptr;
	btBvhTriangleMeshShape* terrainShapePtr = nullptr;
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
	std::vector<GLuint> indices; //for vertices of terrain
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	 float translationX = 0.0f;
	 float translationY = 0.0f;
	 float translationZ = 0.0f;
	 float yScale = 1.0f; // Initialize the Y-axis scale factor
	 btDiscreteDynamicsWorld* dynamicsWorldUniversalPtr = NULL;
	 bool init = false;
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