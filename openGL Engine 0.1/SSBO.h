#pragma once



#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Mesh.h"
class SSBO
{
public:
	SSBO();
	SSBO(GLuint bindingIndex, const void* data, size_t dataSize, GLenum useage);

	void Unbind();
	void Bind();
	void updateData(const void* newData, size_t dataSize);
	void clearSSBO();
	int bindIndex;
	GLuint instanceAmount = 1;
	GLuint ssboID;
	~SSBO();
	GLenum error;

private:
	Mesh* instancedMesh = nullptr;
	//GLuint bindingIndexCount; //curent highest count used only when creating instanced SSBOs at run time, such as grass and trees etc
	std::string SSBOName;
};

