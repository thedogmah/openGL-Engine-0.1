#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <vector>

#include "btBulletDynamicsCommon.h"

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
class Engine
{
};

struct heightmap {

	char* heightData;// height data
	int size; //the height size, must be power of two*
	std::vector<float> heights;
};

class Terrain {


public:
	heightmap heightmapData;
	float heightScale = 0.0; //scaling variable
	std::fstream fstreamObj;

	//height map values;
	float frequency = 0.1f; // Adjust this to control the scale of details
	float amplitude = 1.0f;
public:
	
	Terrain();
	unsigned int* shaderPtr = nullptr;
	//Do I need destructor for parent class?
	int size =0; //must be a power of two
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

	btRigidBody* getTerrainRigidBody();
	btCollisionShape* getTerrainCollionShape();
	btRigidBody* getTerrainMesh();
	glm::vec3 calculateSurfaceNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

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
	GLuint VAO, VBO, EBO, normalBuffer, colorBuffer;
	GLenum error;
	// Reserve space for vertices (assuming m_iSize is the size of your heightmap)


	//Shader variables
	glm::vec3 waterColor, grassColor, rockyColor, snowColor, ambientColor;
	float waterThreshold, grassThreshold, rockyThreshold, snowThreshold;
	float waterStopThreshold = 0.1f; // Adjust the default values as needed
	float grassStopThreshold = 0.3f;
	float rockyStopThreshold = 0.6f;
	float snowStopThreshold = 0.9f;
	float currentTime = 0.0f;

	bool ready = false; //switch to ready when ready for calling render function.
	//physics
	btTriangleMesh* terrainMesh = nullptr;
	btBvhTriangleMeshShape* terrainShapePtr = nullptr;
	btRigidBody* terrainRigidBody = nullptr;
	btRigidBody* terrainMeshRigidBody = nullptr;
	btDefaultMotionState* terrainMotionState = nullptr;
	bool applyModelTransform = false;
	//terrain members
	std::vector<GLfloat> vertices; //vertices of the terrain itself
	std::vector<GLuint> indices; //for vertices of terrain
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	 float translationX = 0.0f;
	 float translationY = 0.0f;
	 float translationZ = 0.0f;
	 btDiscreteDynamicsWorld* dynamicsWorldUniversalPtr = NULL;
	 bool init = false;

};