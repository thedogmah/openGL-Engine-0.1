#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{

public:

	// program ID
	unsigned int ID;


	Shader(const char* vertexPath, const char* fragmentPath);

	template <typename T>
	void setUniform(const std::string name, const T &value) const;
	void use();
	void setProgramID(unsigned int ID);// ID is set in the constructor, returned from gl function
	void checkCompileErrors(GLuint shader, std::string type);
};

