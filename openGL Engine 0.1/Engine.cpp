#include "Engine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
Terrain::Terrain() 
{
	
	
	//GLuint VAO;
	
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//vertices.reserve(size * size);
	this->size = 16; //set a default terrain size.
	this->heightmapData.size =16;
}

void Terrain::render()
{

	//set time for shader functions
	static double previousTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = static_cast<float>(currentTime - previousTime);
	previousTime = currentTime;


	glUseProgram(*this->shaderPtr);
	GLint timeLocation = glGetUniformLocation(*this->shaderPtr, "time");
	glUniform1f(timeLocation, currentTime);
	GLint waterColorLocation = glGetUniformLocation(*this->shaderPtr, "waterColor");
	GLint grassColorLocation = glGetUniformLocation(*this->shaderPtr, "grassColor");
	GLint rockyColorLocation = glGetUniformLocation(*this->shaderPtr, "rockyColor");
	GLint snowColorLocation = glGetUniformLocation(*this->shaderPtr, "snowColor");

	GLint waterThresholdLocation = glGetUniformLocation(*this->shaderPtr, "waterThreshold");
	GLint grassThresholdLocation = glGetUniformLocation(*this->shaderPtr, "grassThreshold");
	GLint rockyThresholdLocation = glGetUniformLocation(*this->shaderPtr, "rockyThreshold");
	GLint snowThresholdLocation = glGetUniformLocation(*this->shaderPtr, "snowThreshold");
	GLint ambientColorLocation = glGetUniformLocation(*this->shaderPtr, "ambientColor");

	GLint waterStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "waterStopThreshold");
	GLint grassStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "grassStopThreshold");
	GLint rockyStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "rockyStopThreshold");
	GLint snowStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "snowStopThreshold");

	//render UI
	ImGui::Begin("Shader Controls");
	// Water Color

	if (ImGui::SliderFloat("Water Stop Threshold", &waterStopThreshold, -20.0f, 70.0f)) {
		glUniform1f(waterStopThresholdLocation, waterStopThreshold);
	}

	if (ImGui::SliderFloat("Grass Stop Threshold", &grassStopThreshold, -20.0f,70.0f)) {
		glUniform1f(grassStopThresholdLocation, grassStopThreshold);
	}

	if (ImGui::SliderFloat("Rocky Stop Threshold", &rockyStopThreshold, -20.0f, 70.0f)) {
		glUniform1f(rockyStopThresholdLocation, rockyStopThreshold);
	}
	if (ImGui::SliderFloat("Snow Stop Threshold", &snowStopThreshold, -20.0f, 70.0f)) {
		glUniform1f(snowStopThresholdLocation, snowStopThreshold);
	}

		if (ImGui::ColorEdit3("Water Color", &waterColor.x)) {
			glUniform3fv(waterColorLocation, 1, &waterColor.x);
		}

	if (ImGui::ColorEdit3("Grass Color", &grassColor.x)) {
		glUniform3fv(grassColorLocation, 1, &grassColor.x);
	}

	if (ImGui::ColorEdit3("Rocky Color", &rockyColor.x)) {
		glUniform3fv(rockyColorLocation, 1, &rockyColor.x);
	}

	if (ImGui::ColorEdit3("Snow Color", &snowColor.x)) {
		glUniform3fv(snowColorLocation, 1, &snowColor.x);
	}
	
	if (ImGui::ColorEdit3("Ambient Color", &ambientColor.x)) {
		glUniform3fv(ambientColorLocation, 1, &ambientColor.x);
	}
	// Height Thresholds
	if (ImGui::SliderFloat("Water Threshold", &waterThreshold, -20.0f, 30.0f)) {
		glUniform1f(waterThresholdLocation, waterThreshold);
	}

	if (ImGui::SliderFloat("Grass Threshold", &grassThreshold, -20.0f, 40.0f)) {
		glUniform1f(grassThresholdLocation, grassThreshold);
	}

	if (ImGui::SliderFloat("Rocky Threshold", &rockyThreshold, -20.0f, 40.0f)) {
		glUniform1f(rockyThresholdLocation, rockyThreshold);
	}

	if (ImGui::SliderFloat("Snow Threshold", &snowThreshold, -20.0f, 40.0f)) {
		glUniform1f(rockyThresholdLocation, snowThreshold);
	}

	ImGui::End();

	ImGui::Begin("Terrain Controls");

	// Input fields for X, Y, and Z coordinates
	GLint applyModelTransform = glGetUniformLocation(*this->shaderPtr, "applyModelTransform");
	glUniform1i(applyModelTransform, 1);
	ImGui::InputFloat("Translate X", &translationX);
	ImGui::InputFloat("Translate Y", &translationY);
	ImGui::InputFloat("Translate Z", &translationZ);
	GLuint modelLoc = glGetUniformLocation(*this->shaderPtr, "model");
	// Button to apply the translation
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	if (ImGui::Button("Apply Translation")) {
		// Update the model matrix with the typed-in values
		 // Identity matrix, no initial transformation
		//modelMatrix = glm::translate(modelMatrix, glm::vec3(translationX, translationY, translationZ));


		for (size_t i = 0; i < vertices.size(); i += 3) {
			vertices[i] += translationX;
			vertices[i + 1] += translationY;
			vertices[i + 2] += translationZ;
		}
		//delete terrainMesh;


		btTransform currentTransform = terrainMeshRigidBody->getWorldTransform();

		// Calculate the new position by adding the translations
		btVector3 currentPosition = currentTransform.getOrigin();
		btVector3 newPosition = btVector3(currentPosition.x() + translationX, currentPosition.y() + translationY, currentPosition.z() + translationZ);

		// Set the new position in the transformation
		currentTransform.setOrigin(newPosition);

		// Update the rigid body's world transform
		terrainMeshRigidBody->setWorldTransform(currentTransform);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		translationX = 0.0;

		translationY = 0.0;

		translationZ = 0.0;



		// Pass the updated model matrix to the shader

	}
	
	if (ImGui::Button("Reset ")) {
		// Update the model matrix with the typed-in values
		  // Identity matrix, no initial transformation
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, 0.0));

		glUniform1i(applyModelTransform, 1);

		// Pass the updated model matrix to the shader


	}

	ImGui::End();

	if (heightmapData.heights.size() > 0) {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		

		//loop through the Z axis of the terrain

			//loop through the X axis of the terrain
			//this is where the triangle strip is constructed
		glDrawElements(GL_TRIANGLES, (this->size) * (this->size) * 6, GL_UNSIGNED_INT, 0);


		//	glBindVertexArray(VAO); // Bind the VAO
		glBindVertexArray(0); // Unbind the VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	//sets an int 'bool' to tell the shader to stop using the terrains model matrix
	glUniform1i(applyModelTransform, 0);

	//if (modelLoc == -1 || viewLoc == -1 || projectionLoc == -1 || applyModelTransformLoc == -1) {
}

void Terrain::initalise()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	init = true;
}




bool Terrain::loadHeightMap(char* filename, int isize)
{

	//check to see if the data is set
	if (heightmapData.heightData)
	{
		unloadHeightMap();
	}
	//allocate the memory for our height data
	heightmapData.heightData = new  char[isize * isize];
	//check to see whether the memory was successfully allocated
	if (heightmapData.heightData == NULL)

	{
	std::cout << "\nMemory not allocated for %s", filename;
	return false;
	}

	//read the height map into context
	fstreamObj.open(filename, std::ios::in | std::ios::binary);
	if (fstreamObj.is_open()) {
		fstreamObj.read(heightmapData.heightData, isize*isize);
		std::cout << "File open";
		//reset cursor position

		//fstreamObj.seekg(0, std::ios::beg);
		
		
			std::cout << heightmapData.heightData;

	}
	else
	{
		std::cout << "File not open";
	}

	heightmapData.size = isize;
	size = heightmapData.size;

	
	fstreamObj.close();

	//createTerrainMesh();
	return true;
	Vertex vertex;
	for (int z = 0; z < size; z++) {
		for (int x = 0; x < size; x++) {
			// Create a vertex with position and color
			
			vertex.position = glm::vec3(x, getScaledHeightAtPoint(x, z), z);
			// You can set the color here based on your requirements
			vertex.color.r = getTrueHeightAtPoint(x, z);
			vertex.color.g = getTrueHeightAtPoint(x, z);
			vertex.color.b = getTrueHeightAtPoint(x, z);
			// White color as an example

		//	vertices.push_back(vertex);
		}

		//set the color with OpenGL, and render the point
	/*	glColor3ub(ucColor, ucColor, ucColor);
	glVertex3f(iX,
		getScaledHeightAtPoint(iX, iZ + 1),
		iZ + 1);*/
	}
	//end the triangle strip
	//while ((error = glGetError()) != GL_NO_ERROR) {
	//	std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	//}
	//glGenVertexArrays(1, &VAO);
	////glBindVertexArray(VAO);
	//// Generate and bind the VBO
	//glGenBuffers(1, &VBO);
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//while ((error = glGetError()) != GL_NO_ERROR) {
	//	std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	//}
	//glBufferData(GL_ARRAY_BUFFER, heightmapData.heights.size() * sizeof(float), heightmapData.heights.data(), GL_STATIC_DRAW);
	//while ((error = glGetError()) != GL_NO_ERROR) {
	//	std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	//}
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(0);
	//while ((error = glGetError()) != GL_NO_ERROR) {
	//	std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	//}
	//std::cout << "\nEnable pointer in loading height map";
	//while ((error = glGetError()) != GL_NO_ERROR) {
	//	std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	//}
	//glEnableVertexAttribArray(0);
	//while ((error = glGetError()) != GL_NO_ERROR) {
	//	std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	//}
	//// Color attribute (location = 1) - optional
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	//std::cout << "\nEnable pointer in loading height map";
	//while ((error = glGetError()) != GL_NO_ERROR) {
	//	std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	//}
	//glEnableVertexAttribArray(1);

	//while ((error = glGetError()) != GL_NO_ERROR) {
	//	std::cout << "OpenGL Error at game loop start: " << error << std::endl;



	//}

	return true;
}

bool Terrain::createTerrainMesh()
{
	vertices.clear();
	normals.clear();
	indices.clear();
	/*std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;*/
	std::vector<GLfloat> colours;
	//std::vector<glm::vec3> normals;

	  // Create a vector to store normals for each vertex.
	const float minThreshold = 0.1f;    // Adjust these thresholds as needed
	const float mountainThreshold = 0.7f;
	const float snowThreshold = 0.9f;
	const glm::vec3 grassyColor = glm::vec3(0.1f, 0.8f, 0.2f);  // Green
	const glm::vec3 mountainColor = glm::vec3(0.5f, 0.4f, 0.3f);  // Dark brown for lower areas
	const glm::vec3 snowColor = glm::vec3(1.0f, 1.0f, 1.0f);     // White for snow


	float scaleX =1.0f; // Scale factor along the X-axis
	float scaleY = 1.0f; // Scale factor along the Y-axis
	float scaleZ = 1.0f; // Scale factor along the Z-axis
	glm::vec3 lowerColor(0.2, 0.2, 0.2);
	glm::vec3 upperColor(0.8, 0.8, 0.8);
	float minHeight = 0.1, maxHeight =0.2;
	// Generate terrain vertices with scaling
	for (int z = 0; z < size; ++z) {
		for (int x = 0; x < size; ++x) {
			float xPos = static_cast<float>(x) * scaleX;
			float zPos = static_cast<float>(z) * scaleZ;
			float yPos = this->heightmapData.heights[x + z * this->size];// Adjust terrain height function if needed

			// Store the scaled vertex position
			vertices.push_back(xPos);
			vertices.push_back(yPos); // Apply Y-axis scaling
			vertices.push_back(zPos);
		
			if (yPos < minHeight)
				minHeight = yPos;
			if (yPos > maxHeight)
				maxHeight = yPos;

			float grayColor = yPos; // You can modify this calculation for better color mapping

			// Calculate a normalized height value between 0 and 1
			float normalizedHeight = (yPos - minHeight) / (maxHeight - minHeight);

			// Interpolate between lowerColor and upperColor based on normalizedHeight
			glm::vec3 finalColor = (1.0f - normalizedHeight) * lowerColor + normalizedHeight * upperColor;

			// Store the color for this vertex
			colours.push_back(finalColor.r);
			colours.push_back(finalColor.g);
			colours.push_back(finalColor.b);

			// Normalize the grayscale color to the range [0, 1]
			//grayColor = (grayColor - minThreshold) / (snowThreshold - minThreshold);

			
			
		}
	}

	std::cout << "min Height was: "<< minHeight;
	std::cout << "\nmax height was: " << maxHeight;
	for (int z = 0; z < this->size - 1; ++z) {
		for (int x = 0; x < this->size - 1; ++x) {
			int topLeft = x + z * this->size;
			int topRight = (x + 1) + z * this->size;
			int bottomLeft = x + (z + 1) * this->size;
			int bottomRight = (x + 1) + (z + 1) * this->size;

			indices.push_back(topLeft);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(bottomRight);
			indices.push_back(bottomLeft);
		}
	}
	std::vector<glm::vec3> Normals(vertices.size(), glm::vec3(0.0f));

	

	// Calculate normals for triangles and accumulate them into vertex normals
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		GLuint i0 = indices[i];
		GLuint i1 = indices[i + 1];
		GLuint i2 = indices[i + 2];

		glm::vec3 v0(vertices[i0 * 3], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]);
		glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
		glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

		// Calculate the triangle's normal
		glm::vec3 triangleNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		// Accumulate the triangle's normal to the vertex normals
		Normals[i0] += triangleNormal;
		Normals[i1] += triangleNormal;
		Normals[i2] += triangleNormal;
	}

	// Normalize all vertex normals
	for (size_t i = 0; i < Normals.size(); ++i)
	{
		Normals[i] = glm::normalize(Normals[i]);
	}

		
		// Create OpenGL buffers
	
		glBindVertexArray(VAO);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		// Specify vertex attribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(GLfloat), colours.data(), GL_STATIC_DRAW);
	
		glEnableVertexAttribArray(1);
		// Specify vertex attribute pointer for colors (location = 1)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		
		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(glm::vec3), Normals.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		std::vector<float> normalizedHeights;
		normalizedHeights.reserve(heightmapData.heights.size());

		// Calculate the height scaling factor
		float heightScale = 1.0f / (maxHeight - minHeight);

		// Normalize and copy the heights
		minHeight = maxHeight = heightmapData.heights[0];

		// Normalize and copy the heights, while updating min and max heights
		for (float height : heightmapData.heights) {
			if (height < minHeight) {
				minHeight = height;
			}
			if (height > maxHeight) {
				maxHeight = height;
			}

			float normalizedHeight = (height - minHeight) / (maxHeight - minHeight);
			normalizedHeights.push_back(normalizedHeight);
		}

		std::vector<float> heightData2;
		for (int i = 1; i < vertices.size(); i += 3) {
			heightData2.push_back(vertices[i]);
		}
		 terrainMesh = new btTriangleMesh();

		// Populate the btTriangleMesh with your vertex and index data
		 
		for (size_t i = 0; i < indices.size(); i += 3) {
			// Get the vertices of the current triangle
			btVector3 vertex1(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
			btVector3 vertex2(vertices[indices[i + 1] * 3], vertices[indices[i + 1] * 3 + 1], vertices[indices[i + 1] * 3 + 2]);
			btVector3 vertex3(vertices[indices[i + 2] * 3], vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 2] * 3 + 2]);

			// Add the triangle to the btTriangleMesh
			terrainMesh->addTriangle(vertex1, vertex2, vertex3);
		}

		// Create a btBvhTriangleMeshShape using the btTriangleMesh
		 terrainShapePtr = new btBvhTriangleMeshShape(terrainMesh, true, true);

		// Set the local scaling if needed
		btVector3 localScaling(1.0f, 1.0f, 1.0f);
		terrainShapePtr->setLocalScaling(localScaling);

		// Create a btRigidBodyConstructionInfo as before
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(0.0, 0.0, 0.0));
		btDefaultMotionState* terrainMotionState = new btDefaultMotionState(startTransform);

		btRigidBody::btRigidBodyConstructionInfo terrainRigidBodyCI(
			0.0f,                      // Mass (0 for static object)
			terrainMotionState,
			terrainShapePtr,           // Use the btBvhTriangleMeshShape
			btVector3(0, 0, 0)        // Local inertia (0 for static object)
		);

		// Create the btRigidBody as before
		 terrainMeshRigidBody = new btRigidBody(terrainRigidBodyCI);
		 dynamicsWorldUniversalPtr->addRigidBody(terrainMeshRigidBody);
		// Set any additional properties for the terrainRigidBody if needed

		// Mark your terrain as ready
		ready = true;

		std::cout << "Heightmap Data:" << std::endl;

	/////	for (int i = 0; i < vertices.size(); i += 3) {
	//		std::cout << "Vertex " << i / 3 << ": (" << vertices[i] << ", " << vertices[i + 1] << ", " << vertices[i + 2] << ")" << std::endl;
	//	}
	//	std::cout << "\n";
	////	for (int i = 0; i < heightmapData.heights.size(); i += 3) {
	////		std::cout << "Height " << i / 3 << ": " << heightmapData.heights[i] << ", " << heightmapData.heights[i + 1] << ", " << heightmapData.heights[i + 2] << std::endl;
	//	}
	return true;
}

bool Terrain::unloadHeightMap()
{

	if (heightmapData.heightData)
	{
		//delete the data
		delete[] heightmapData.heightData;//delete checks if it is null pointer, so the check is not necessary

		this->size = 0;
		heightmapData.size = 0;
	}
	//the heightmap has been unloaded
	std::cout << "Successfully unloaded the heightmap\n";
	return true;
}

void Terrain::generateHeightMap()
{
	/*if (heightmapData.size* heightmapData.size != heightmapData.heights.size()) {
		heightmapData.heights.resize(heightmapData.size * heightmapData.size);
	}*/

	this->heightmapData.heights.clear();
	this->heightmapData.heights.resize(this->size * this->size);
	std::cout << "\nSize of heightmap is: " << this->heightmapData.heights.size();
	// frequency = 0.1f; // Adjust this to control the scale of details
	 //amplitude = 1.0f; // Adjust this to control the overall height range
	std::cout << "Begin terrain generation in generate Height Map Function\n";
		for (int y = 0; y < size; ++y) {
			for (int x = 0; x < size; ++x) {
				int index = x + y * size;
				float u = static_cast<float>(x) / static_cast<float>(size - 1);
				float v = static_cast<float>(y) / static_cast<float>(size - 1);

				// Generate height using Perlin noise
			  // Generate height using Perlin noise with adjusted parameters
				float height = glm::simplex(glm::vec2(u * frequency, v * frequency)) * amplitude;

				this->heightmapData.heights[index] = height;
			}
		}

		std::cout << "Begin terrain generation in generate Height Map Function\n";

	}



btRigidBody* Terrain::getTerrainRigidBody()
{
	if (terrainRigidBody == nullptr) std::cout << "Null Pointer in getTerrainRigidBody";
	return this->terrainRigidBody;
}

btCollisionShape* Terrain::getTerrainCollionShape()
{
	if (terrainShapePtr == nullptr) std::cout << "Null Pointer in getTerrainCollisionShape";
	return this->terrainShapePtr;
}

btRigidBody* Terrain::getTerrainMesh()
{if (terrainMeshRigidBody != nullptr)
		return this->terrainMeshRigidBody;
}

glm::vec3 Terrain::calculateSurfaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
	glm::vec3 edge1 = v2 - v1;
	glm::vec3 edge2 = v3 - v1;


	// Calculate the unnormalized normal
	glm::vec3 normal = glm::cross(edge1, edge2);

	// Ensure the normal is pointing upwards (positive y-component)
	if (normal.y < 0.0f) {
		normal = -normal; // Reverse the normal direction
	}
	//std::cout << "\nNormal before normalised: " << normal.x << ", " << normal.y << ", " << normal.z;
	normal= glm::normalize(normal);
	//std::cout << "\tNormal after normalised: " << normal.x << ", " << normal.y << ", " << normal.z;
	return normal;
}


//
//// Define your TerrainData and HeightMapData structures
//struct HeightMapData {
//	int size;
//	std::vector<float> heights;
//};
//
//struct TerrainData {
//	int size;
//	float frequency;
//	float amplitude;
//	HeightMapData heightmapData;
//};
//
//TerrainData terrain;
//
//// GLFW window and input callback functions
//GLFWwindow* window;
//
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
//		glfwSetWindowShouldClose(window, GLFW_TRUE);
//	}
//}
//
//// OpenGL shader programs and rendering functions
//GLuint shaderProgram;
//GLuint VAO, VBO, EBO;
//
//void createTerrainMesh() {
//	// Create vertex and index data based on the heightmap
//	// This code assumes a regular grid of vertices based on terrain.size
//
//	std::vector<GLfloat> vertices;
//	std::vector<GLuint> indices;
//
//	for (int z = 0; z < terrain.size; ++z) {
//		for (int x = 0; x < terrain.size; ++x) {
//			float xPos = static_cast<float>(x);
//			float zPos = static_cast<float>(z);
//			float yPos = terrain.heightmapData.heights[x + z * terrain.size];
//
//			vertices.push_back(xPos);
//			vertices.push_back(yPos);
//			vertices.push_back(zPos);
//		}
//	}
//
//	for (int z = 0; z < terrain.size - 1; ++z) {
//		for (int x = 0; x < terrain.size - 1; ++x) {
//			int topLeft = x + z * terrain.size;
//			int topRight = (x + 1) + z * terrain.size;
//			int bottomLeft = x + (z + 1) * terrain.size;
//			int bottomRight = (x + 1) + (z + 1) * terrain.size;
//
//			indices.push_back(topLeft);
//			indices.push_back(topRight);
//			indices.push_back(bottomLeft);
//			indices.push_back(topRight);
//			indices.push_back(bottomRight);
//			indices.push_back(bottomLeft);
//		}
//	}
//
//	// Create OpenGL buffers
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//	glGenBuffers(1, &VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
//
//	glGenBuffers(1, &EBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
//
//	// Specify vertex attribute pointers
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
//	glEnableVertexAttribArray(0);
//
//	glBindVertexArray(0);
//}
//
//void initShaders() {
//	// Vertex shader source code
//	const char* vertexShaderSource = R"(
//        #version 330 core
//        layout (location = 0) in vec3 position;
//        uniform mat4 model;
//        uniform mat4 view;
//        uniform mat4 projection;
//        void main() {
//            gl_Position = projection * view * model * vec4(position, 1.0);
//        }
//    )";
//
//	// Fragment shader source code
//	const char* fragmentShaderSource = R"(
//        #version 330 core
//        out vec4 color;
//        void main() {
//            color = vec4(0.0, 1.0, 0.0, 1.0); // Green color for terrain (you can change this)
//        }
//    )";
//
//	// Compile shaders
//	GLuint vertexShader, fragmentShader;
//	vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//	glCompileShader(vertexShader);
//
//	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//	glCompileShader(fragmentShader);
//
//	// Create shader program
//	shaderProgram = glCreateProgram();
//	glAttachShader(shaderProgram, vertexShader);
//	glAttachShader(shaderProgram, fragmentShader);
//	glLinkProgram(shaderProgram);
//
//	// Delete shaders (we don't need them anymore after linking)
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//}
//
//void renderTerrain(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
//	glUseProgram(shaderProgram);
//	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
//	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
//	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//
//	glBindVertexArray(VAO);
//	glDrawElements(GL_TRIANGLES, (terrain.size - 1) * (terrain.size - 1) * 6, GL_UNSIGNED_INT, 0);
//	glBindVertexArray(0);
//}