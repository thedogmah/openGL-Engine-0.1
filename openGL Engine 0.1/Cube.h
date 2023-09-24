#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "worldObject.h"
#include "cubeData.h"
class Cube : public worldObject
{
public:
	Cube(GLuint shaderProgramID, float size = 1.0, bool useTexture = false);
	void draw();
	void setScale(float scale);
	void update();
	void setCubeData(cubeData cubeData);
	//function to set texture
	void setTexture(GLuint textureID);
	//void deleteRigidBody();

	GLuint VAO, VBO, EBO;
	std::vector <float> vertices;
	std::vector <unsigned int> indices;

	cubeData cubeProperties;

	bool useTexture;
	GLuint textureID;
	glm::mat4 modelMatrix;
	GLint modelMatrixLoc;
	GLuint shaderProgramID;
	float scale =1.0;
	void setupMesh();
	


};

