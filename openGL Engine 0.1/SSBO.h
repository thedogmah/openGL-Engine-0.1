#pragma once

#include "globals.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
class SSBO
{
public:

	SSBO(GLuint bindingIndex, const void* data, size_t dataSize, GLenum useage);

	void Unbind();
	void Bind();
	void updateData(const void* newData, size_t dataSize);
	void clearSSBO();
	int bindIndex;
	GLuint ssboID;
	~SSBO();
	GLenum error;
};

