#include "CubeMap.h"
#include <iostream>
#include "stb/stb_image.h" // For textur


CubeMap::CubeMap() : cubemapShader("skybox.vert", "skybox.frag")
{
	glGenTextures(1, &textureID);
	//unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	// Bind VAO and VBO, load vertex data from the vector
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

	// Use &skyboxVertices[0] to get the pointer to the first element
	glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float), &skyboxVertices[0], GL_STATIC_DRAW);

	// Specify vertex attribute pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0); // Unbind VAO


	this->faces = {
	   "px.png", // Positive X (right face)
	   "nx.png", // Negative X (left face)
	   "py.png", // Positive Y (top face)
	   "ny.png", // Negative Y (bottom face)
	   "pz.png", // Positive Z (front face)
	   "nz.png"  // Negative Z (back face)
	};


}

CubeMap::~CubeMap()
{
	glDeleteTextures(1, &textureID);
}

void CubeMap::loadCubeMap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < faces.size(); i++) {
		loadTexture(faces[i], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		std::cout << "loading textures loop for sky cubemap";
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void CubeMap::bind(unsigned int unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void CubeMap::unbind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

unsigned int CubeMap::getID() const
{
	return textureID;
}

unsigned int CubeMap::loadTexture(const std::string& path, unsigned int face)
{
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	std::cout << "Load texture function for cubemap class";
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(face, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		logger.AddLog("Loaded CubeBox Texture");
		std::cout << "loading skybox textures";
	}

	 else {
	 std::cout << "\nFailed to load cubebox texture: " << path << std::endl;
	 logger.AddLog("Failed to load cubebox texture. See console for path.");
	}
	return textureID;


}
void CubeMap::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	// Skybox-specific adjustments
	glDepthFunc(GL_LEQUAL); // Ensure skybox depth is drawn correctly
	glDepthMask(GL_FALSE);  // Disable depth writing

	// Use the cubemap shader
	cubemapShader.use();
	float timeValue = glfwGetTime();
	// Set view and projection matrices in the shader
	glm::mat4 view = glm::mat4(glm::mat3(viewMatrix)); // Remove translation part from view matrix
	cubemapShader.setUniform("view", view);            // Send view matrix
	cubemapShader.setUniform("time", timeValue);
	cubemapShader.setUniform("projection", projectionMatrix); // Send projection matrix
	cubemapShader.setUniform("sunColor", sun.DiffuseColor);
	// Bind the cubemap texture to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); // Make sure textureID is the cubemap texture

	// Set the "skybox" uniform
	cubemapShader.setUniform("skybox", 0); // Texture unit 0
	glViewport(0, 0, window_width, window_height); //oriinally skybox was rendering somewhat smaller than the main screen. I'm not sure where and why this changes
	//m(above) maybe in callbackc function or initial setup
	// Bind the VAO and draw the skybox
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0); // Unbind VAO

	// Unbind cubemap texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// Reset depth function
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);   // Re-enable depth writing

}
