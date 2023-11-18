#pragma once



#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Mesh.h"
#include "Lights.h"
class Mesh;
class Lights;
struct cubeInstance;
class SSBO
{
public:
	SSBO();
	SSBO(GLuint bindingIndex, const void* data, size_t dataSize, GLenum useage);

	void Unbind();
	void Bind();
	void updateData(const void* newData, size_t dataSize);
	void clearSSBO();
	void setDataVector(std::vector<World::cubeInstance> data);
	std::vector<World::cubeInstance> getDataVector();
	int bindIndex;
	GLuint instanceAmount = 1;
	GLuint ssboID;
	~SSBO();
	GLenum error;

	Mesh* instancedMesh = nullptr;
	bool operator<(const SSBO& other) const
	{
		// Implement a logic for comparing instances, for example, based on ssboID
		return ssboID < other.ssboID;
	}

	std::string SSBOName;
	std::vector<World::cubeInstance>instanceDataVector;

private:
	
	//GLuint bindingIndexCount; //curent highest count used only when creating instanced SSBOs at run time, such as grass and trees etc
	

};

