#include "SSBO.h"

SSBO::SSBO(GLuint bindingIndex, const void* data, size_t dataSize, GLenum useage)
{

	glGenBuffers(1, &ssboID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);
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
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, newData, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

SSBO::~SSBO()
{
	//glDeleteBuffers(1, &ssboID);
}
