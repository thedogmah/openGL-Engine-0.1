#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "globals.h"
class Loader
{
public:
	static GLuint loadToVao(const std::vector<GLfloat>& positions,
		const std::vector<GLfloat>& textureCoords,
		const std::vector<GLfloat>& normals,
		const std::vector<GLuint>& indices);

	static GLuint loadToVao(std::vector<GLfloat>& positions, int dimensions);

	static GLuint loadTexture(const std::string& textureFile);

	static GLuint loadCubeMap(const std::vector<std::string>& textureFiles);




	static void bindIndicesBuffer(const std::vector<GLuint>& indices);

	static void storeDataInAttributeList(int attributeNumber, int coordinateSize, const std::vector<GLfloat>& data);
};

