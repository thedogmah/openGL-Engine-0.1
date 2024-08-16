#pragma once
//#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "globals.h"
#include <glm/glm.hpp>

class Tessellation
{
public:
	Tessellation();
	~Tessellation();
	void init();
	void draw();
	void setMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix);
	bool checkShaderCompilation(GLint shader); 
	GLenum error;
private:

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	GLuint TessShaderProgram;
	int numVertices;
	int numIndices;
	float vertices[12];
	unsigned int indices[];

	
	void setupQuad();
	void setupTriangle();
	void setupShaders();
};

