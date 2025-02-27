#pragma once
#include "globals.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp >
#include <vector>
#include "Shader.h"

class Gizmo : public Shader
	
{
public:
	Gizmo();
	~Gizmo();

	void Draw();

	GLFWwindow* window = nullptr;
private:
	struct Arrow {
		glm::vec3 direction;
		glm::vec3 color;

	};
	
	std::vector<Arrow>arrows;
	unsigned int VAO, VBO;

	bool isRotating = false;
	int selectedAxis = -1; // -1 = none, 0 = X, 1 = Y, 2 = Z
	glm::vec2 mouseInitial; // Initial mouse position
	glm::mat4 modelInitial; // Initial model matrix
	glm::mat4 modelStart;
	glm::mat4 model; // Pointer to the model matrix to rotate

	//gizmo debugging
	float baseThreshold = 0.5f; // Base distance threshold
	float scaleM = 0.1f;       // Scaling factor for dynamic threshold
	bool gizmoRotate = false;  //switch for rotating / translating.(moving)
	void setupBuffers();

	//helpers
	void handleInput(GLFWwindow* window, const glm::mat4& view, const glm::mat4& projection, float width, float height);
	float rayLineDistance(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lineStart, const glm::vec3& lineEnd);

	//void setupShader();

};

