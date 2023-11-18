#include "SSBO.h"

SSBO::SSBO()
{
}

SSBO::SSBO(GLuint bindingIndex, const void* data, size_t dataSize, GLenum useage)
{
	bindIndex = bindingIndex;
	glGenBuffers(1, &this->ssboID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->ssboID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, data, useage);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssboID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::Bind() {

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);	

}

void SSBO::Unbind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::updateData(const void* newData, size_t dataSize) 
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->ssboID);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	}
	glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, newData, GL_STATIC_DRAW);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	}
}

void SSBO::clearSSBO()
{
	//GLuint ssbo;
	//glGenBuffers(1, &ssboID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindIndex, ssboID);
	std::cout << "\nssbo object ID: " << ssboID << std::endl;
}

void SSBO::setDataVector(std::vector<World::cubeInstance> data)
{
	this->instanceDataVector = data;
}

std::vector<World::cubeInstance> SSBO::getDataVector()
{
	return this->instanceDataVector;
}

SSBO::~SSBO()
{
	//glDeleteBuffers(1, &ssboID);
}
