#pragma once
#include "globals.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp >
#include <vector>
#include "Shader.h"
#include "globals.h"
#include <deque>
struct Ray {

	glm::vec3 origin; //starting point in world space
	glm::vec3 direction;// normalised direction in world space
	glm::vec3 end;      //end point for visualisation
	glm::vec3 color;    //RGB color of the ray


};
class Gizmo : public Shader
	
{
public:
	Gizmo();
	~Gizmo();

	void Draw();

	GLFWwindow* window = nullptr;

	Ray clickRay = {

	glm::vec3(0.0f, 0.0f, 0.0f),    // Origin at (0,0,0)
	glm::vec3(0.0f, 0.0f, 1.0f),    // Direction along positive Z
	glm::vec3(0.0f, 0.0f, 100.0f),  // End point 100 units along Z
	glm::vec3(1.0f, 0.0f, 0.5f)     // Purple color

	};

	float rayLength = 150.0f; // Length of the ray for visualization



private:
	struct Arrow {
		glm::vec3 direction;
		glm::vec3 color;

	};

	void onMouseClick(int mouseX, int mouseY, int screenWidth, int screenHeight, Camera& camera);

	float pointToLineSegmentDistance(const glm::vec2& point, const glm::vec2& lineStart, const glm::vec2& lineEnd);

	std::deque<Ray>rays;
	std::vector<Arrow>arrows;
	unsigned int VAO, VBO;
	GLuint rayVAO, rayVBO;
	bool isRotating = false;
	int selectedAxis = -1; // -1 = none, 0 = X, 1 = Y, 2 = Z
	glm::vec2 mouseInitial; // Initial mouse position
	glm::mat4 modelInitial; // Initial model matrix
	glm::mat4 modelStart;
	glm::mat4 model; //The model matrix to rotate

	//gizmo debugging
	float baseThreshold = 1.5f; // Base distance threshold
	float scaleM = 0.1f;       // Scaling factor for dynamic threshold
	bool gizmoRotate = false;  //switch for rotating / translating.(moving)
	bool renderRay = true; // Flag to toggle ray rendering
	void setupBuffers();

	//helpers
	void handleInput(GLFWwindow* window, const glm::mat4& view, const glm::mat4& projection,
		int windowWidth, int windowHeight);


	//void setupShader();

};

