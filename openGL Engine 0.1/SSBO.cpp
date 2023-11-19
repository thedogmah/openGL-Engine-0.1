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

void SSBO::setRotation(int item, const glm::vec3& rotation)
{
	if (item >= 0 && item < instanceDataVector.size()) {
		// Access the model matrix
		glm::mat4& modelMatrix = instanceDataVector[item].modelMatrix;

		// Decompose the existing model matrix
		glm::vec3 existingTranslation, existingScale;
		glm::quat existingRotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(modelMatrix, existingScale, existingRotation, existingTranslation, skew, perspective);

		// Apply the new rotation to the existing rotation
		glm::quat newRotation = glm::quat(glm::radians(rotation));
		glm::quat finalRotation = newRotation * existingRotation;

		// Reconstruct the model matrix with the updated rotation
		modelMatrix = glm::translate(glm::mat4(1.0f), existingTranslation) *
			glm::mat4_cast(finalRotation) *
			glm::scale(glm::mat4(1.0f), existingScale);
	}

	// Update the SSBO data if needed
	this->updateData(this->instanceDataVector.data(), sizeof(this->instanceDataVector[0]) * this->instanceDataVector.size());
}
void SSBO::setTranslation(int item, const glm::vec3& translation)
{
	if (item >= 0 && item < instanceDataVector.size()) {
		// Access the model matrix
		glm::mat4& modelMatrix = instanceDataVector[item].modelMatrix;

		// Decompose the existing model matrix
		glm::vec3 existingTranslation, existingScale;
		glm::quat existingRotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(modelMatrix, existingScale, existingRotation, existingTranslation, skew, perspective);

		// Update the translation component
		existingTranslation = translation;

		// Reconstruct the model matrix with the updated translation
		modelMatrix = glm::translate(glm::mat4(1.0f), existingTranslation) *
			glm::mat4_cast(existingRotation) *
			glm::scale(glm::mat4(1.0f), existingScale);
	}

	// Update the SSBO data if needed
	this->updateData(this->instanceDataVector.data(), sizeof(this->instanceDataVector[0]) * this->instanceDataVector.size());


}
void SSBO::setScale(int item, const glm::vec3& scale)
{
	if (item >= 0 && item < instanceDataVector.size()) {
		// Access the model matrix
		glm::mat4& modelMatrix = instanceDataVector[item].modelMatrix;

		// Decompose the existing model matrix
		glm::vec3 existingTranslation, existingScale;
		glm::quat existingRotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(modelMatrix, existingScale, existingRotation, existingTranslation, skew, perspective);

		// Update the scale component
		existingScale = scale;

		// Reconstruct the model matrix with the updated scale
		modelMatrix = glm::translate(glm::mat4(1.0f), existingTranslation) *
			glm::mat4_cast(existingRotation) *
			glm::scale(glm::mat4(1.0f), existingScale);
	}

	// Update the SSBO data if needed
	this->updateData(this->instanceDataVector.data(), sizeof(this->instanceDataVector[0]) * this->instanceDataVector.size());

}
std::vector<World::cubeInstance> SSBO::getDataVector()
{
	return this->instanceDataVector;
}

SSBO::~SSBO()
{
	//glDeleteBuffers(1, &ssboID);
}
