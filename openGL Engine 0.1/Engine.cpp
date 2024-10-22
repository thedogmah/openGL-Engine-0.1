#include "Engine.h"
#include <glad/glad.h>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "stb_image_write.h"
#include <stb/stb_image.h>
#include "globals.h"
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <algorithm>

Terrain::Terrain() 
{
	
	
	//GLuint VAO;
	
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//vertices.reserve(size * size);
	this->size = 128; //set a default terrain size.
	this->heightmapData.size =128;
	std::mt19937 gen(rd()); //for generate river variables.

	currentTerrainClickedRGB.clear();//Clear the vector before adding latest picked locations (X,Y,Z)
	currentTerrainClickedRGB.push_back(255.0);//Add red green and blue to vector
	currentTerrainClickedRGB.push_back(255.0);
	currentTerrainClickedRGB.push_back(255.0);


}

void Terrain::render()
{

	//Create a variable that only draws the IMGUI once.
	
	
	//
	// 
	// 
	// 
	// Define a random number generator and initialize it with a seed


		 	GLenum error;
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error at engine::render start: " << error << std::endl;

			}

	//rng.seed(123);     // Set a fixed seed for repeatability (change this for variability)
	//set time for shader functions
	static double previousTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = static_cast<float>(currentTime - previousTime);
	previousTime = currentTime;
//	glUseProgram(*this->shaderPtr);
	renderTextureLoader();
	if (UIdrawn == false) {
		
		if (drawIMGUI) {
			ImGui::Begin("Terrain Settings");


			ImGui::Begin("Scale");
			if (ImGui::SliderFloat("Y Scale", &yScale, 0.5f, 10.0f)) {

				for (int i = 1; i < vertices.size(); i += 3) {
					// Update the Y-coordinate by scaling it
					vertices[i] *= yScale;

					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

				}
			}		// Update the Y-axis scale factor when the slider changes

			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error at ::render after binding buffer: " << error << std::endl;
			}

			ImGui::End();

			// Create input fields for the values
			ImGui::InputInt("Iterations", &terrainIterations);
			ImGui::InputFloat("Height", &terrainHeight);
			ImGui::InputFloat("Min Delta", &terrainMinDelta);
			ImGui::InputFloat("Max Delta", &terrainMaxDelta);
			ImGui::InputInt("Smooth Iterations", &numIterations);
			ImGui::InputFloat("Kernal Size", &kernelPoint);
			ImGui::Checkbox("Fractalise Terrain", &boolFractalTerrain);
			ImGui::Checkbox("Voxelate Terrain", &boolVoxelateTerrain);
			ImGui::Checkbox("FIR Erosion Terrain", &boolFIRErosion);
			ImGui::Checkbox("Trim Terrain Edges", &boolTrimEdges);
			ImGui::SliderFloat("Mountain Scaling", &mountain_scaling, 0.0, 100);  // Adjust to control mountain height



			if (ImGui::Button("Generate Terrain")) {
				// Call terrain generation function with the updated values
				//
				//GenerateFractalTerrain(vertices, terrainIterations, terrainHeight, terrainMinDelta, terrainMaxDelta);
				dynamicsWorldUniversalPtr->removeCollisionObject(this->getTerrainMesh());

				createTerrainMesh();
			}
			ImGui::End();

			ImGui::Checkbox("Use Normal Map", &useNormalMap);
			ImGui::Checkbox("Use Detail Map", &useDetailMap);

			GLint riverPathIndicesLocation = glGetUniformLocation(*shaderPtr, "riverPathIndices");
			GLint riverPathIndicesSizeLocation = glGetUniformLocation(*shaderPtr, "riverPathIndicesSize");
			GLint riverBedLocation = glGetUniformLocation(*shaderPtr, "riverBed");

			// Assuming riverPath is a std::vector<int> containing the generated river path indices
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error at ::render near start: " << error << std::endl;
			}

			glUniform1iv(riverPathIndicesLocation, riverPath.size(), reinterpret_cast<const GLint*>(riverPath.data()));
			glUniform1i(riverPathIndicesSizeLocation, riverPath.size());

			// ...
			GLint useNormalMapLocation = glGetUniformLocation(*this->shaderPtr, "useNormalMap");
			GLint useDetailMapLocation = glGetUniformLocation(*this->shaderPtr, "useDetailMap");
			// Later in your rendering code where you set the uniform
			glUniform1i(useNormalMapLocation, useNormalMap);
			glUniform1i(useDetailMapLocation, useDetailMap);

			GLint radiansTimeLocation = glGetUniformLocation(*this->shaderPtr, "radian");
			glUniform1f(radiansTimeLocation, radiansTime);
			GLint waterColorLocation = glGetUniformLocation(*this->shaderPtr, "waterColor");
			GLint grassColorLocation = glGetUniformLocation(*this->shaderPtr, "grassColor");
			GLint rockyColorLocation = glGetUniformLocation(*this->shaderPtr, "rockyColor");
			GLint snowColorLocation = glGetUniformLocation(*this->shaderPtr, "snowColor");
			GLint diffuseColorLocation = glGetUniformLocation(*this->shaderPtr, "diffuseColor");
			GLint diffuseColorLocationWater = glGetUniformLocation(*globalWaterShader, "diffuseColor");
			GLint smoothStepLocation = glGetUniformLocation(*this->shaderPtr, "smoothStepFactor");
			GLint waterThresholdLocation = glGetUniformLocation(*this->shaderPtr, "waterThreshold");
			GLint grassThresholdLocation = glGetUniformLocation(*this->shaderPtr, "grassThreshold");
			GLint rockyThresholdLocation = glGetUniformLocation(*this->shaderPtr, "rockyThreshold");
			GLint snowThresholdLocation = glGetUniformLocation(*this->shaderPtr, "snowThreshold");
			GLint peakThresholdLocation = glGetUniformLocation(*this->shaderPtr, "peakThreshold");
			GLint ambientColorLocation = glGetUniformLocation(*this->shaderPtr, "ambientColor");

			GLint waterStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "waterStopThreshold");
			GLint grassStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "grassStopThreshold");
			GLint rockyStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "rockyStopThreshold");
			GLint snowStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "snowStopThreshold");
			GLint shininessLocation = glGetUniformLocation(*this->shaderPtr, "shininess");
			GLint specularColorLocation = glGetUniformLocation(*this->shaderPtr, "specularColor");
			GLint sunBrightnessLocation = glGetUniformLocation(*this->shaderPtr, "sunBrightness");

			GLint lightDirectionLocation = glGetUniformLocation(*this->shaderPtr, "lightDirection");

			//render UI
			ImGui::Begin("Shader Controls");
			// Water Color
			float lowestY = -1;  // Start with a very high value
			float highestY = 1; // Start with a very low value

			//// Iterate through the Y-coordinates of 'vertices'
			//for (int i = 1; i < vertices.size(); i += 3) {
			//	float y = vertices[i]; // Y-coordinate for this vertex

			//	// Update lowest and highest Y-values
			//	if (y < lowestY) {
			//		lowestY = y;
			//	}
			//	if (y > highestY) {
			//		highestY = y;
			//	}
			//}

			// Use ImGui to display the lowest and highest Y-values as text labels
			ImGui::Text("Lowest Y: %.2f", lowestY);
			ImGui::Text("Highest Y: %.2f", highestY);

			ImGui::Text("Shader Uniforms:");

			// Assuming your shader program is already active
			GLint heightScaleLocation = glGetUniformLocation(*this->shaderPtr, "heightScale");
			GLint heightOffsetLocation = glGetUniformLocation(*this->shaderPtr, "heightOffset");
			GLint slopeThresholdLocation = glGetUniformLocation(*this->shaderPtr, "slopeThreshold");

			// Control for heightScale
			if (ImGui::SliderFloat("River Bed", &riverBedValue, -10.0f, 10.0f)) {
				// Assuming shaderPtr is your shader program pointer
				GLint riverBedLocation = glGetUniformLocation(*shaderPtr, "riverBed");
				glUseProgram(*shaderPtr);
				glUniform1f(riverBedLocation, riverBedValue);
			}
			if (ImGui::SliderFloat("Slope Threshold", &slopeThreshold, -1.0f, 1.0f)) {		// Update the shader uniform
				glUniform1f(slopeThresholdLocation, slopeThreshold);
			}

			if (ImGui::SliderFloat("Slope Threshold2", &slopeThreshold, 10.0f, 1000.0f)) {		// Update the shader uniform
				glUniform1f(slopeThresholdLocation, slopeThreshold);
			}
			std::cout << "\nslope threshold: " << slopeThreshold;
			ImGui::SliderFloat("Height Scale", &theightScale, 0.1f, 30.0f);
			// Update the shader uniform
			glUniform1f(heightScaleLocation, theightScale);


			ImGui::SliderFloat("Height Offset", &heightOffset, -10.0f, 20.0f);
			// Update the shader uniform
			glUniform1f(heightOffsetLocation, heightOffset);
			if (ImGui::SliderFloat("Water Stop Threshold", &waterStopThreshold, -220.0f, 570.0f)) {
				glUniform1f(waterStopThresholdLocation, waterStopThreshold);
			}

			if (ImGui::SliderFloat("Grass Stop Threshold", &grassStopThreshold, -220.0f, 570.0f)) {
				glUniform1f(grassStopThresholdLocation, grassStopThreshold);
			}

			if (ImGui::SliderFloat("Rocky Stop Threshold", &rockyStopThreshold, -220.0f, 570.0f)) {
				glUniform1f(rockyStopThresholdLocation, rockyStopThreshold);
			}
			if (ImGui::SliderFloat("Snow Stop Threshold", &snowStopThreshold, -220.0f, 570.0f)) {
				glUniform1f(snowStopThresholdLocation, snowStopThreshold);
			}

			/*if (ImGui::InputFloat3("- Light Direction - ", &lightDirection.x)) {
				glUniform3fv(lightDirectionLocation, 1, &lightDirection.x);
			}*/

			if (ImGui::ColorEdit3("- Light Colour - ", &diffuseColor.x)) {
				glUniform3fv(diffuseColorLocation, 1, &diffuseColor.x);
				glProgramUniform3fv(*globalWaterShader, diffuseColorLocationWater, 1, &diffuseColor.x);
				sun.DiffuseColor = diffuseColor;
			}

			if (ImGui::SliderFloat("Shininess", &shininess, 0.2f, 100.0f)) {
				glUniform1f(shininessLocation, shininess);

			}
			if (ImGui::SliderFloat("Sun Brightness", &sunBrightness, 0.1, 2.0f)) {
				glUniform1f(sunBrightnessLocation, sunBrightness);
				sun.Brightness = sunBrightness;
			}

			//if (ImGui::SliderFloat("Time of Day", &timeOfDay, 0.0f, 24.0f)) {
			//	// Update the sun's position whenever the time of day changes
			//	// You can use this callback to trigger any other time-dependent effects
			//	 sunX = glm::clamp(-std::abs(timeOfDay - 12.0f) / 6.0f + 1.0f, -1.0f, 1.0f);
			//	 sunY = glm::clamp(-std::abs(timeOfDay - 12.0f) / 6.0f + 1.0f, -1.0f, 1.0f);

			//	sunPosition = glm::vec3(sunX, -sunY, 0.0f);
			//	glUniform3fv(lightDirectionLocation, 1, &sunPosition.x);

			//}
				//timeOfDay = 18.0;
			if (ImGui::SliderFloat("Time", &radiansTime, 0.0f, 360.0f)) {

				glUniform1f(radiansTimeLocation, radiansTime);
				sun.radiansTime = radiansTime;
			}
			ImGui::Text("Sun Position: (%.2f, %.2f, %.2f)", sunPosition.x, sunPosition.y, sunPosition.z);

			//need to make ambience white at midday and black at midnight.
			//shininess needs to go a lot higher


			// Color picker for specularColor
			if (ImGui::ColorEdit3("Specular Color", &specularColor.x))
			{
				glUniform3fv(specularColorLocation, 1, &specularColor.x);
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
			if (ImGui::SliderFloat("Water Threshold", &waterThreshold, terrainBottom, terrainTop)) {
				glUniform1f(waterThresholdLocation, waterThreshold);
			}

			if (ImGui::SliderFloat("Grass Threshold", &grassThreshold, terrainBottom, terrainTop)) {
				glUniform1f(grassThresholdLocation, grassThreshold);
			}

			if (ImGui::SliderFloat("Rocky Threshold", &rockyThreshold, terrainBottom, terrainTop)) {
				glUniform1f(rockyThresholdLocation, rockyThreshold);
			}

			if (ImGui::SliderFloat("Snow Threshold", &snowThreshold, terrainBottom, terrainTop)) {
				glUniform1f(snowThresholdLocation, snowThreshold);
			}
			if (ImGui::SliderFloat("Peak Threshold", &peakThreshold, terrainBottom, terrainTop)) {
				glUniform1f(peakThresholdLocation, peakThreshold);
			}

			if (ImGui::SliderFloat("!Smooth Step", &smoothStep, 0.0f, 150.0f)) {
				glUniform1f(smoothStepLocation, smoothStep);
			}

			ImGui::End();
		}
	}
	ImGui::Begin("Terrain Controls");

	// Input fields for X, Y, and Z coordinates
	GLint applyModelTransform = glGetUniformLocation(*this->shaderPtr, "applyModelTransform");
	glUniform1i(applyModelTransform, 1);//this variable tells shader its terain
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

	if (ImGui::Button("Center Terrain")) {
		glUniform1i(applyModelTransform, 1);
		glm::vec3 center(0.0f, 0.0f, 0.0f); // Initialize a vector to store the center
		for (size_t i = 0; i < vertices.size(); i += 3) {
			center.x += vertices[i];
			center.y += vertices[i + 1];
			center.z += vertices[i + 2];
		}
		center /= (vertices.size() / 3); // Divide by the number of vertices to get the average position

		// Calculate the translation needed to move the center to the local origin
		float translationX = -center.x;
		float translationY = -center.y;
		float translationZ = -center.z;

		// Apply the translation to all the vertices
		for (size_t i = 0; i < vertices.size(); i += 3) {
			vertices[i] += translationX;
			vertices[i + 1] += translationY;
			vertices[i + 2] += translationZ;
		}
		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

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
	}
	if (ImGui::Button("Reset ")) {
		// Update the model matrix with the typed-in values
		  // Identity matrix, no initial transformation


	

	
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, 0.0));

		glUniform1i(applyModelTransform, 1);

		// Pass the updated model matrix to the shader


	}

	ImGui::End();

	static int inputX = 0;
	static int inputZ = 0;	
	static bool showHeight = false;
	static unsigned char heightValue = 0;

	ImGui::Begin("Height Check");

	// Input fields for (x, z) coordinates
	ImGui::InputInt("X", &inputX);
	ImGui::InputInt("Z", &inputZ);

	if (!heightmapData.heights.empty()) {
		// Call your getTrueHeightAtPoint function
		heightValue = getTrueHeightAtPoint(inputX, inputZ);

		// Display the Y value
		ImGui::Text("Height at (%d, %d): %u", inputX, inputZ, heightValue);
	}
	else {
		// Display a message if heights vector is empty
		ImGui::Text("Heights vector is empty");
	}

	ImGui::End();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, detailMapTexture);
	
	// Set the uniform in your shader to the texture unit index for the detail map
	glUniform1i(glGetUniformLocation(*this->shaderPtr, "detailMap"), 11); // Use texture unit 2 for the detail map
	
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, mudMapTexture);
	glUniform1i(glGetUniformLocation(*this->shaderPtr, "mudTexture"), 10); // Use texture unit 2 for the detail map

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, mudHeight);
	glUniform1i(glGetUniformLocation(*this->shaderPtr, "mudHeight"), 12); // Use texture unit 2 for the detail map

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, mudNormals);
	glUniform1i(glGetUniformLocation(*this->shaderPtr, "mudNormals"), 13); // Use texture unit 2 for the detail map

	// Set the uniform in your shader to the texture unit index (e.g., 1)
	//glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(*this->shaderPtr, "normalMap"), 1);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at ::render just before DRAW: " << error << std::endl;
	}



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
		//glBindTexture(GL_TEXTURE_2D, 0);
		GLint maxTextureUnits = 0;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

		// Iterate over all texture units and unbind any texture bound to them
		for (GLint i = 0; i < maxTextureUnits; ++i) {
			glActiveTexture(GL_TEXTURE0 + i);  // Activate each texture unit
			glBindTexture(GL_TEXTURE_2D, 0);   // Unbind the 2D texture from that texture unit
		}

		// Restore the default active texture unit (optional but good practice)
		glActiveTexture(GL_TEXTURE0);
	}



	//sets an int 'bool' to tell the shader to stop using the terrains model matrix
	glUniform1i(applyModelTransform, 0);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at ::render end: " << error << std::endl;
	}
	//if (modelLoc == -1 || viewLoc == -1 || projectionLoc == -1 || applyModelTransformLoc == -1) {

	UIdrawn = true;
}

void Terrain::initalise()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &uvVBO);
	initPickingBuffer();

	init = true;
	rockyColor = glm::vec3(0.5, 0.5, 0.5);      // Gray color
	ambientColor = glm::vec3(0.2, 0.2, 0.2);    // Subtle grayish ambient light
	diffuseColor = glm::vec3(1.0, 1.0, 1.0);    // White diffuse light
	specularColor = glm::vec3(1.0, 1.0, 1.0);   // White specular light
	waterColor = glm::vec3(0.0, 0.2, 0.8);      // Blue water color
	snowColor = glm::vec3(1.0, 1.0, 1.0);       // White snow color
	grassColor = glm::vec3(0.1, 0.75, 0.1);     // Green grass color
	GLint waterColorLocation = glGetUniformLocation(*this->shaderPtr, "waterColor");
	GLint grassColorLocation = glGetUniformLocation(*this->shaderPtr, "grassColor");
	GLint rockyColorLocation = glGetUniformLocation(*this->shaderPtr, "rockyColor");
	GLint snowColorLocation = glGetUniformLocation(*this->shaderPtr, "snowColor");
	GLint diffuseColorLocation = glGetUniformLocation(*this->shaderPtr, "diffuseColor");


	GLint waterThresholdLocation = glGetUniformLocation(*this->shaderPtr, "waterThreshold");
	GLint grassThresholdLocation = glGetUniformLocation(*this->shaderPtr, "grassThreshold");
	GLint rockyThresholdLocation = glGetUniformLocation(*this->shaderPtr, "rockyThreshold");
	GLint snowThresholdLocation = glGetUniformLocation(*this->shaderPtr, "snowThreshold");
	GLint ambientColorLocation = glGetUniformLocation(*this->shaderPtr, "ambientColor");

	GLint waterStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "waterStopThreshold");
	GLint grassStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "grassStopThreshold");
	GLint rockyStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "rockyStopThreshold");
	GLint snowStopThresholdLocation = glGetUniformLocation(*this->shaderPtr, "snowStopThreshold");
	GLint shininessLocation = glGetUniformLocation(*this->shaderPtr, "shininess");
	GLint specularColorLocation = glGetUniformLocation(*this->shaderPtr, "specularColor");

	GLint lightDirectionLocation = glGetUniformLocation(*this->shaderPtr, "lightDirection");


	if (ImGui::ColorEdit3("Specular Color", &specularColor.x))
	{
		glUniform3fv(specularColorLocation, 1, &specularColor.x);
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
	// 	GLenum error;
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
	GLuint error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error at start of create terrain mesh function: " << error << std::endl;
	}
	int detailMapWidth, detailMapHeight, detailMapChannels;
	unsigned char* detailMapData = stbi_load("pmossheight.jpg", &detailMapWidth, &detailMapHeight, &detailMapChannels, 0);

	// Generate and bind the texture
	glGenTextures(1, &detailMapTexture);
	glBindTexture(GL_TEXTURE_2D, detailMapTexture);

	if (detailMapData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, detailMapWidth, detailMapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, detailMapData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		// Handle texture loading error
		std::cerr << "Failed to load detail map" << std::endl;
	}


	//mudHeight = loadTexture("mudheight.jpg");
	glGenTextures(1, &mudMapTexture);
	glBindTexture(GL_TEXTURE_2D, mudMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int mudMapWidth, mudMapHeight, mudMapChannels;
	unsigned char* mudMapData = stbi_load("mud2.jpg", &mudMapWidth, &mudMapHeight, &mudMapChannels, 0);

	if (mudMapData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mudMapWidth, mudMapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mudMapData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		// Handle texture loading error
		std::cerr << "Failed to load mud map" << std::endl;
	}
	glGenTextures(1, &mudHeight);
	glBindTexture(GL_TEXTURE_2D, mudHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int mudHeightWidth, mudHeight, mudHeightChannels;
	unsigned char* mudHeightData = stbi_load("mudheight.jpg", &mudHeightWidth, &mudHeight, &mudHeightChannels, 0);

	if (mudHeightData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mudHeightWidth, mudHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mudHeightData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		// Handle texture loading error
		std::cerr << "Failed to load mud h map" << std::endl;
	}

	glGenTextures(1, &mudNormals);
	glBindTexture(GL_TEXTURE_2D, mudNormals);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int mudNormalWidth, mudNormalHeight, mudNormalChannels;
	unsigned char* mudNormalData = stbi_load("mudNormals.jpg", &mudNormalWidth, &mudNormalHeight, &mudNormalChannels, 0);

	if (mudHeightData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mudNormalWidth, mudNormalHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mudNormalData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		// Handle texture loading error
		std::cerr << "Failed to load mud h map" << std::endl;
	}

	// Free the image data
	stbi_image_free(detailMapData);
	vertices.clear();
	normals.clear();
	indices.clear();
	terrainInfoVector.clear();
	verticesID.clear();//Unique IDs for terrain picking

	
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
	TerrainInfo terrainEntry;
	
	for (int z = 0; z < size; ++z) {
		for (int x = 0; x < size; ++x) {
			float xPos = static_cast<float>(x) * scaleX;
			float zPos = static_cast<float>(z) * scaleZ;
			float yPos = this->heightmapData.heights[x + z * this->size];// Adjust terrain height function if needed

			// Store the scaled vertex position
			vertices.push_back(xPos);
			vertices.push_back(yPos); // Apply Y-axis scaling
			vertices.push_back(zPos);
			float id = xPos + yPos;
			verticesID.push_back(id);//This will createa unique number since these values
			//increase, then we just need the range
			//Below we are creating one 'terrainEntry' for each vertices of terrain this allows to then edit these vertices
			//and allocate some of them as water etc. This data is passed to the vertex shader in to a 'location attrib buffer'
			//and only one entry per 3 vertices is required, since 3 vertices corrosponds to one height, or one x,y,z
			terrainInfoVector.push_back(terrainEntry);
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

	// Find the minimum and maximum values
	float minID = 0;
	float maxID = 1;



	fractalTerrain();
	voxelateTerrain();
	firSmoothTerrain();
	mountainsTerrain();
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

		

	//glm::vec3 center(0.0f, 0.0f, 0.0f); // Initialize a vector to store the center
	//for (size_t i = 0; i < vertices.size(); i += 3) {
	//	center.x += vertices[i];
	//	center.y += vertices[i + 1];
	//	center.z += vertices[i + 2];
	//}
	//center /= (vertices.size() / 3); // Divide by the number of vertices to get the average position

	//// Calculate the translation needed to move the center to the local origin   
	//float translationX = -center.x;
	//float translationY = -center.y;
	//float translationZ = -center.z;

	//// Apply the translation to all the vertices
	//for (size_t i = 0; i < vertices.size(); i += 3) {
	//	vertices[i] += translationX;
	//	vertices[i + 1] += translationY;
	//	vertices[i + 2] += translationZ;
	//}


	glm::vec3 transformedVertex = glm::vec3(modelMatrix * glm::vec4(vertices[0], vertices[1], vertices[2], 1.0f));

	terrainBottom = transformedVertex.y;
	terrainTop = transformedVertex.y;

	for (int i = 0; i + 2 < vertices.size(); i += 3)
	{
		// Transform the vertex using the model matrix
		transformedVertex = modelMatrix * glm::vec4(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);

		if (transformedVertex.y < terrainBottom)
			terrainBottom = transformedVertex.y;
		else if (transformedVertex.y > terrainTop)
			terrainTop = transformedVertex.y;
	}

	//We need to loop through the vertices, each X<Y<Z and turn it into a unique value that is noralised (between 0 and 1,
	//and pass that number to the colour

	for (int i = 0; i ==1; i+=3) {
	//	std::cout << "\nBefore normalisation: " << i;//

		float minValue = 0.1;// *std::min_element(verticesID.begin(), verticesID.end());
		float maxValue = 1.0;// *std::max_element(verticesID.begin(), verticesID.end());

		std::transform(verticesID.begin(), verticesID.end(), verticesID.begin(),
			[&](float value) { return (value - minValue) / (maxValue - minValue); });
	//	std::cout << "\tValue: " << verticesID[i];
	
	}
	createUVs();



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

		glGenBuffers(1, &uvVBO);
		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at start of terrain mesh binding : " << error << std::endl;
		}
		glEnableVertexAttribArray(4);
		//glEnableVertexAttribArray(5);
	//	glEnableVertexAttribArray(6);
	//	glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferData(GL_ARRAY_BUFFER, terrainInfoVector.size() * sizeof(TerrainInfo), terrainInfoVector.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(4, 1, GL_INT, GL_FALSE, sizeof(TerrainInfo), (void*)offsetof(TerrainInfo, isWater));
		glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(TerrainInfo), (void*)offsetof(TerrainInfo, isMountain));
		glVertexAttribPointer(6, 1, GL_INT, GL_FALSE, sizeof(TerrainInfo), (void*)offsetof(TerrainInfo, isForest));
		glVertexAttribPointer(7, 1, GL_INT, GL_FALSE, sizeof(TerrainInfo), (void*)offsetof(TerrainInfo, isDesert));
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at start of create terrain mesh function: " << error << std::endl;
		}

		//Vertice IDs sent in as floats, normalised. Phys.H file has the code for the shaders.
		glGenBuffers(1, &verticesIDVBO);
		glBindBuffer(GL_ARRAY_BUFFER, verticesIDVBO);
		glBufferData(GL_ARRAY_BUFFER, verticesID.size() * sizeof(GLfloat), verticesID.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid*)0);
		
		glEnableVertexAttribArray(9);
		glGenBuffers(1, &TerrainClickedRGB_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, TerrainClickedRGB_VBO);
		glBufferData(GL_ARRAY_BUFFER, currentTerrainClickedRGB.size() * sizeof(GLfloat), currentTerrainClickedRGB.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glVertexAttribDivisor(9, 1);
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
		terrainMesh = new btTriangleMesh();

		// Populate the btTriangleMesh with your vertex and index data
		// Create the btHeightfieldTerrainShape using your heightmap data
		terrainShapePtr = new btHeightfieldTerrainShape(
			size,                 // Width of the heightmap (number of points along X axis)
			size,                 // Length of the heightmap (number of points along Z axis)
			&heightmapData.heights,      // Pointer to your heightmap height data
			1.0f,                           // Height scale (usually 1.0f, adjust as needed)
			0,                      // Minimum height value
			12,                      // Maximum height value
			1,                              // Up axis (Y-axis in this case, so it's 1)
			PHY_FLOAT,                      // Data type of height (here, float)
			false                           // FlipQuadEdges (usually false)
		);

		// Optionally set scaling if needed (adjust this as per your terrain size)
		btVector3 localScaling(1.0, 1.0f, 1.0);  // Scale for X, Z axes (Y scale is managed by heightmap heights)
		terrainShapePtr->setLocalScaling(localScaling);
		logger.AddLog(std::to_string(minHeight),ImGuiLogger::LogType::Physics );
		logger.AddLog(std::to_string(minHeight), ImGuiLogger::LogType::Physics);
		// Now create the rigid body for the terrain
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(0.0, 0.0, 0.0));  // Position the terrain at origin

		btDefaultMotionState* terrainMotionState = new btDefaultMotionState(startTransform);

		// Create the rigid body construction info
		btRigidBody::btRigidBodyConstructionInfo terrainRigidBodyCI(
			0.0f,  // Mass of 0, because it's static
			terrainMotionState,
			terrainShapePtr,
			btVector3(0, 0, 0)  // Local inertia is zero for static objects
		);

		// Create the terrain rigid body
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
		calculateGradients(this->heightmapData.heights, this->size, this->size, gradientX, gradientY);

		createNormalMap(this->gradientX, this->gradientY, this->size, this->size, this->normalMap);
		glGenTextures(1, &normalMapTexture);
		glBindTexture(GL_TEXTURE_2D, normalMapTexture);

		// Set texture parameters (you might need to adjust these based on your requirements)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Provide the normal map data to the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->size, this->size, 0, GL_RGB, GL_FLOAT, normalMap.data());

		// Unbind the normal map texture
		glBindTexture(GL_TEXTURE_2D, 0);
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

	
	std::vector<float> octaveAmplitudes(numOctaves);
	std::vector<float> octaveFrequencies(numOctaves);

	for (int i = 0; i < numOctaves; ++i) {
		octaveAmplitudes[i] = pow(persistance, -i); // Persistance Decreasing amplitude with each octave
		octaveFrequencies[i] = pow(lacunarity, i); // Lacunarity Increasing frequency with each octave 

	/*	octaveAmplitudes[i] = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
		octaveFrequencies[i] = std::uniform_real_distribution<float>(1.0f, 5.0f)(rng);*/ // Adjust the frequency range
	}

	this->heightmapData.heights.clear();
	this->heightmapData.heights.resize(this->size * this->size, 0.0f);

	for (int y = 0; y < size; ++y) {
		for (int x = 0; x < size; ++x) {
			int index = x + y * size;
			float u = static_cast<float>(x) / static_cast<float>(size - 1);
			float v = static_cast<float>(y) / static_cast<float>(size - 1);
			float height = 0.0f; // Initialize height to zero

			for (int i = 0; i < numOctaves; ++i) {
				height += glm::simplex(glm::vec2(u * octaveFrequencies[i], v * octaveFrequencies[i])) * octaveAmplitudes[i];
			}

			this->heightmapData.heights[index] = height * amplitude;
		}
	}
//Consider having noise in the range of -1 to 1 (result *  2 - 1)
}

std::pair<int, int> Terrain::indexToCoordinates(int index)
{
	int i = index / this->size;
	int j = index % this->size;
	return std::make_pair(i, j);
}

int Terrain::coordinatesToIndex(int i, int j)
{
	return i * this->size + j;
}

int Terrain::findHighestPoint(const std::vector<GLfloat>& vertices)
{
	size_t numVertices = vertices.size() / 3; // Assuming vertices are in groups of three (X, Y, Z)

	if (numVertices == 0) {
		// Handle the case where there are no vertices
		return -1; // Or some other sentinel value
	}

	// Initialize variables to keep track of the maximum Y value and its index
	float maxY = vertices[1]; // Initial maximum Y value
	int maxYIndex = 0; // Initial index of the maximum Y value

	// Iterate through the Y values
	for (size_t i = 1; i < numVertices; ++i) {
		float currentY = vertices[i * 3 + 1]; // Assuming Y values are at indices 1, 4, 7, ...

		// Check if the current Y value is greater than the maximum Y value
		if (currentY > maxY) {
			maxY = currentY;
			maxYIndex = static_cast<int>(i + 1); // Update the index
		}
	}

	std::cout << "Max Heigiht is: " << vertices[maxYIndex];
	return maxYIndex;
}

void Terrain::createUVs()
{
	// Initialize variables to store minimum and maximum vertex positions for UV calculation
	   // Initialize variables to store minimum and maximum vertex positions
	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::min();

	uvs.clear();

	// Calculate the minimum and maximum values of the x and z coordinates
	for (size_t i = 0; i < vertices.size(); i += 3) {
		float x = vertices[i + 0]; // Access x component
		float y = vertices[i + 1]; // Access y component
		float z = vertices[i + 2]; // Access z component

		if (x < minX) minX = x;
		if (x > maxX) maxX = x;
		if (z < minZ) minZ = z;
		if (z > maxZ) maxZ = z;
	}

	// Calculate UV coordinates based on the vertex position
	for (int i = 0; i < vertices.size(); i += 3) {
		// The vertex data layout assumes [x, y, z, x, y, z, ...]
		float x = vertices[i];
		float z = vertices[i + 2];

		// Calculate UVs based on the vertex position
		float u = (x - minX) / ((maxX - minX) / 1);
		float v = (z - minZ) / ((maxZ - minZ) / 1);

		// Store the UV coordinates
		uvs.push_back(glm::vec2(u, v));
	}
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

void Terrain::fractalTerrain()
{
	if (boolFractalTerrain) {
	
		vertices = GenerateFractalTerrain(vertices, terrainIterations, terrainHeight, terrainMinDelta, terrainMaxDelta);
	}
}

void Terrain::voxelateTerrain()
{
	if (boolVoxelateTerrain)
	{
		const int gridWidth = size;
		const int gridHeight = size;
		const int gridDepth = size;
		const float voxelSize = 0.1;

		// Create an empty voxel grid
		std::vector<std::vector<std::vector<Voxel>>> voxelGrid(
			gridWidth, std::vector<std::vector<Voxel>>(
				gridHeight, std::vector<Voxel>(gridDepth, { false }) // Initialize all voxels as not occupied
			)
		);

		// Example terrain vertices (replace this with your terrain data)
	//	std::vector<std::vector<std::vector<float>>> vertices;

		// Loop through terrain vertices and voxelize
		for (size_t i = 0; i < vertices.size(); i += 3) {
			float x = vertices[i];
			float y = vertices[i + 1];
			float z = vertices[i + 2];

			// Calculate voxel indices for the current vertex
			int voxelX = static_cast<int>(x / voxelSize);
			int voxelY = static_cast<int>(y / voxelSize);
			int voxelZ = static_cast<int>(z / voxelSize);

			// Mark the voxel as occupied (part of the terrain)
			if (voxelX >= 0 && voxelX < gridWidth &&
				voxelY >= 0 && voxelY < gridHeight &&
				voxelZ >= 0 && voxelZ < gridDepth) {
				voxelGrid[voxelX][voxelY][voxelZ].isOccupied = true;
			}
		}
	}
}

void Terrain::firSmoothTerrain()
{
	if (boolFIRErosion) {
		std::vector<float> verticesBackup;
		verticesBackup = vertices;
		if (boolFIRErosion) {
			// FIR SMOOTHING
			int terrainSizeX = size /* Width of your terrain */;
			int terrainSizeY = size /* Height of your terrain */;

			// Define a simple 3x3 FIR smoothing kernel
			float kernel[3][3] = {
				{1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
				{1.0f / 9.0f, 1.0f / 9.0, 1.0f / 9.0f},
				{1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f}
			};

			// Iterate through all vertices and apply smoothing twice (forward and backward)
			for (int iteration = 0; iteration < numIterations; ++iteration) {
				// Forward smoothing pass (left to right, top to bottom)
				for (int y = 0; y < terrainSizeY-3; ++y) {
					for (int x = 0; x < terrainSizeX-3; ++x) {
						int currentIndex = (y * terrainSizeX + x) * 3;
						float sum = 0.0f;

						// Apply the filter to the vertex using the kernel
						for (int ky = -1; ky <= 1; ++ky) {
							for (int kx = -1; kx <= 1; ++kx) {
								int nx = x + kx;
								int ny = y + ky;

								// Ensure boundary conditions are met
								if (nx >= 0 && nx < terrainSizeX && ny >= 0 && ny < terrainSizeY) {
									int neighborIndex = ((ny * terrainSizeX + nx) * 3) + 1; // Assuming 3 components per vertex (XYZ), and targeting the Y-component
									sum += vertices[neighborIndex] * kernel[ky + 1][kx + 1];
								}
							}
						}

						// Update the Y-component of the current vertex with the smoothed value
						vertices[currentIndex + 1] = sum;
					}
				}

				// Backward smoothing pass (right to left, bottom to top)
				for (int y = terrainSizeY - 3; y >= 0; --y) {
					for (int x = terrainSizeX -3; x >= 0; --x) {
						int currentIndex = (y * terrainSizeX + x) * 3;
						float sum = 0.0f;

						// Apply the filter to the vertex using the kernel
						for (int ky = -1; ky <= 1; ++ky) {
							for (int kx = -1; kx <= 1; ++kx) {
								int nx = x + kx;
								int ny = y + ky;

								// Ensure boundary conditions are met
								if (nx >= 0 && nx < terrainSizeX && ny >= 0 && ny < terrainSizeY) {
									int neighborIndex = ((ny * terrainSizeX + nx) * 3) + 1; // Assuming 3 components per vertex (XYZ), and targeting the Y-component
									sum += vertices[neighborIndex] * kernel[ky + 1][kx + 1];
								}
							}
						}

						// Update the Y-component of the current vertex with the smoothed value
						vertices[currentIndex + 1] = sum;
					}
				}
			}

			// Ensure boundary vertices stay unchanged (comment out if not needed)
			// Ensure boundary vertices stay unchanged (comment out if not needed)
		// Ensure boundary vertices stay unchanged (comment out if not needed)
		
			if (boolTrimEdges) {

				for (int x = 0; x < terrainSizeX; ++x) {
					int rowIndex = x * 3;  // Calculate the index for the Y-value in the current row
					vertices[rowIndex + 1] = 0.0f;  // Set the Y-value to zero
				}
				//for (int x = 0; x < terrainSizeX; ++x) {
				//	int topBoundaryIndex = x * 3; // Top boundary vertex
				//	int bottomBoundaryIndex = ((terrainSizeY - 1) * terrainSizeX + x) * 3; // Bottom boundary vertex

				//	vertices[topBoundaryIndex + 1] = verticesBackup[topBoundaryIndex + 1];
				//	vertices[bottomBoundaryIndex + 1] = verticesBackup[bottomBoundaryIndex + 1];
				//}

				//for (int y = 0; y < terrainSizeY; ++y) {
				//	int leftBoundaryIndex = (y * terrainSizeX) * 3; // Left boundary vertex
				//	int rightBoundaryIndex = ((y * terrainSizeX) + (terrainSizeX - 1)) * 3; // Right boundary vertex

				//	vertices[leftBoundaryIndex + 1] = verticesBackup[leftBoundaryIndex + 1];
				//	vertices[rightBoundaryIndex + 1] = verticesBackup[rightBoundaryIndex + 1];
				//}

				//// Now, restore the vertices at the very edge of all four sides
				//int topLeftCornerIndex = 0; // Top-left corner vertex
				//int topRightCornerIndex = (terrainSizeX - 1) * 3; // Top-right corner vertex
				//int bottomLeftCornerIndex = ((terrainSizeY - 1) * terrainSizeX) * 3; // Bottom-left corner vertex
				//int bottomRightCornerIndex = ((terrainSizeY - 1) * terrainSizeX + terrainSizeX - 1) * 3; // Bottom-right corner vertex

				//vertices[topLeftCornerIndex + 1] = verticesBackup[topLeftCornerIndex + 1];
				//vertices[topRightCornerIndex + 1] = verticesBackup[topRightCornerIndex + 1];
				//vertices[bottomLeftCornerIndex + 1] = verticesBackup[bottomLeftCornerIndex + 1];
				//vertices[bottomRightCornerIndex + 1] = verticesBackup[bottomRightCornerIndex + 1];
			}
		}
	}


	


	
}

void Terrain::generateRiver(std::vector<GLfloat>& vertices, int startPointIndex, int finalPointIndex, int numMidPoints)
{
	int width, height, channels;
	unsigned char* image = stbi_load("water.jpg", &width, &height, &channels, 3);
	if (!image) {
		// Handle texture loading failure
		std::cerr << "Failed to load water texture" << std::endl;
		return;
	}
	glGenTextures(1, &waterTextureID);
	glBindTexture(GL_TEXTURE_2D, waterTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		// Handle OpenGL error
		std::cerr << "OpenGL error: " << error << std::endl;

		// Clean up and return
		stbi_image_free(image);
		glDeleteTextures(1, &waterTextureID);
		return;
	}
	stbi_image_free(image);
	riverPath.clear();

	// Generate a random endpoint
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> endPointDistribution(0, vertices.size() / 3 - 1);
	finalPointIndex = endPointDistribution(gen);

	riverPath.push_back(startPointIndex);

	// Generate random midpoints
	std::vector<int> midpoints;
	midpoints.push_back(startPointIndex);

	for (int i = 0; i < numMidPoints; ++i) {
		int randomIndex = endPointDistribution(gen) * 3;  // Assuming vertices are in the format x, y, z
		midpoints.push_back(randomIndex / 3);
	}

	// Connect midpoints to create the path
	for (size_t i = 0; i < midpoints.size() - 1; ++i) {
		int currentPoint = midpoints[i];
		int nextPoint = midpoints[i + 1];

		// Create the path by adding each step (index) along the way
		int deltaX = nextPoint % size - currentPoint % size;
		int deltaY = nextPoint / size - currentPoint / size;
		int steps = std::max(std::abs(deltaX), std::abs(deltaY));

		for (int step = 1; step <= steps; ++step) {
			int stepX = currentPoint % size + step * deltaX / steps;
			int stepY = currentPoint / size + step * deltaY / steps;

			// Ensure stepX and stepY are within bounds
			stepX = std::max(0, std::min(size - 1, stepX));
			stepY = std::max(0, std::min(size - 1, stepY));

			int stepIndex = stepY * size + stepX;
			riverPath.push_back(stepIndex);
			terrainInfoVector[stepIndex].isWater = 1;  // Update the terrainInfoVector for the water flag

			// Lower the Y value by -10 for the corresponding vertex in the vertices vector
			vertices[stepIndex * 3 + 1] -= 10.0f;
			vertices[stepIndex * 3 + 2] -= 10.0f; // Assuming vertices are in the format x, y, z
		}
	}

	// Connect the last midpoint to the final point
	riverPath.push_back(finalPointIndex);
	terrainInfoVector[finalPointIndex].isWater = 1;

	glBindVertexArray(VAO);


	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, terrainInfoVector.size() * sizeof(TerrainInfo), terrainInfoVector.data(), GL_STATIC_DRAW);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, terrainInfoVector.size() * sizeof(TerrainInfo), terrainInfoVector.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::mountainsTerrain()
{
	for (int i = 0; i < vertices.size(); i += 3) {
		float x = vertices[i];
		float z = vertices[i + 2];

		// Generate mountain noise at the current (x, z) position
		float u = static_cast<float>(x) / static_cast<float>(size - 1);
		float v = static_cast<float>(z) / static_cast<float>(size - 1);
		float mountain_noise = glm::simplex(glm::vec2(u * frequency, v * frequency));

		// Scale the mountain noise to control mountain height
		float mountain_height = mountain_noise * mountain_scaling;

		// Add mountains to the terrain by modifying the y-coordinate
		vertices[i + 1] += mountain_height;
	}
}

void Terrain::renderTextureLoader()
{



	ImGui::Begin("Rivers and Paths");

	if ((ImGui::Button("Generate River"))){
		//riverPath.clear();
		std::srand(std::time(0));

		// Initialize terrain vertices
		if (vertices.size() > 0) {
			// Generate a river from the highest point to a logical final point
			int startPointIndex = findHighestPoint(vertices);

			// Set the final point to the maximum height vertex
			//int finalPointIndex = findMaxHeightPoint(vertices);

			// Adjust the number of midpoints as needed
			int numMidPoints = 10;

			generateRiver(vertices, startPointIndex, 1, numMidPoints);
		}
	

	}

	if ((ImGui::Button("Widen River"))) {
		size_t ss = riverPath.size();
		for (size_t i = 0; i < ss; ++i) {
			// Add the original float
			float originalFloat = riverPath[i];

			// Add the second float (original + givenNumber)
			terrainInfoVector[i + this->size].isWater = 1;
			riverPath.push_back(originalFloat + this->size);
		}
	}
	ImGui::Text("River Path Indices:");
	/*for (int index : riverPath) {
		ImGui::Text("%d", index);
	}*/



	ImGui::End();
	ImGui::Begin("Texture Manager");
	
	// Load Texture button
	ImGui::InputText("File Name", fileNameBuffer, sizeof(fileNameBuffer));
	//printf("File Name Buffer Contents: %s\n", fileNameBuffer);
	// Load Texture button
	if (ImGui::Button("Load Texture")) {
		// Get the file name from the input field
		const char* texturePath = fileNameBuffer;

		if (strlen(texturePath) > 0) {
			GLuint newTexture = loadTexture(texturePath);
			if (newTexture != 0 && boolTextureLoadSuccess) {
				textureIDs.push_back(newTexture);
				textureNames.push_back(texturePath);
				selectedTextureIndex = textureIDs.size() - 1;
			}
		}
	}

	if (ImGui::Button("Clear Texture")) { textureIDs.clear(); textureNames.clear(); }

	// List of currently loaded textures
	for (int i = 0; i < textureIDs.size(); i++) {
		ImGui::Separator();
		ImGui::Text("Texture %d: %s", i, textureNames[i].c_str());

		// Display a thumbnail (you need to implement this part)
		// You can use ImGui::Image to display thumbnails here

		// Checkbox to select the texture
		bool isSelected = (i == selectedTextureIndex);
		if (ImGui::Checkbox(("Select##" + std::to_string(i)).c_str(), &isSelected)) {
			if (isSelected) {
				selectedTextureIndex = i;
			}
			else if (selectedTextureIndex == i) {
				selectedTextureIndex = -1; // Deselect the texture
			}
		}
	}

	if (textureIDs.size() > 0) {
		if (textureIDs.size() > 0 && textureIDs[0] != 0) {
			logger.AddLog("inside texture function");
			std::cout << "inside texture functdion";
			int uniformTexture = glGetUniformLocation(*this->shaderPtr, "txGrass");
			glActiveTexture(GL_TEXTURE2); // Texture unit 0
			glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
			glUniform1i(uniformTexture, 2);
		}

		if (textureIDs.size() > 1 && textureIDs[1] != 0) {
			int uniformTexture = glGetUniformLocation(*this->shaderPtr, "txHighGrass");
			glActiveTexture(GL_TEXTURE3); // Texture unit 1
			glBindTexture(GL_TEXTURE_2D, textureIDs[1]);
			glUniform1i(uniformTexture, 3);
		}

		if (textureIDs.size() > 2 && textureIDs[2] != 0) {
			int uniformTexture = glGetUniformLocation(*this->shaderPtr, "txRock");
			glActiveTexture(GL_TEXTURE4); // Texture unit 2
			glBindTexture(GL_TEXTURE_2D, textureIDs[2]);
			glUniform1i(uniformTexture, 4);
		}

		if (textureIDs.size() > 3 && textureIDs[3] != 0) {
			int uniformTexture = glGetUniformLocation(*this->shaderPtr, "txHighRock");
			glActiveTexture(GL_TEXTURE5); // Texture unit 3
			glBindTexture(GL_TEXTURE_2D, textureIDs[3]);
			glUniform1i(uniformTexture,5);
		}

		if (textureIDs.size() > 4 && textureIDs[4] != 0) {
			int uniformTexture = glGetUniformLocation(*this->shaderPtr, "txPeak");
			glActiveTexture(GL_TEXTURE6); // Texture unit 4
			glBindTexture(GL_TEXTURE_2D, textureIDs[4]);
			glUniform1i(uniformTexture, 6);

		}
	}
	// Activate texture unit 0 (you can choose a different one if needed)
	glActiveTexture(GL_TEXTURE7);

	// Bind the water texture to texture unit 0
	glBindTexture(GL_TEXTURE_2D, waterTextureID);

	// Set the waterTexture uniform in the shader to use texture unit 0
	GLint waterTextureLoc = glGetUniformLocation(*this->shaderPtr, "waterTexture");
	glUniform1i(waterTextureLoc, 0);
	ImGui::End(); // Texture Manager window

}

GLuint Terrain::loadTexture(const char* path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, numChannels;

	unsigned char* data = stbi_load(path, &width, &height, &numChannels, 0);
	if (data) {
	//if Texture loaded
		GLenum format = (numChannels == 3) ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Set Texture Wrapping and Filtering options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		boolTextureLoadSuccess = true;
	}

	else {
		std::cout << "Failed to load texture:" << path << std::endl;
		stbi_image_free(data);
		boolTextureLoadSuccess = false;
	}
	return textureID;
}

	std::vector<GLfloat> Terrain::GenerateFractalTerrain(std::vector<GLfloat>& Vertices,
		int iIterations,
		float iHeight,
		float iMinDelta,
		float iMaxDelta) {
		// Create a copy of Vertices to store the fractal terrain

		float deltaHeight = iMaxDelta - iMinDelta;
		std::vector<GLfloat> FractalVertices = Vertices;


		//find the range
		float minY = FLT_MAX;
		float maxY = -FLT_MAX;
		for (int i = 1; i < Vertices.size(); i += 3) {
			float y = Vertices[i];
			if (y < minY) {
				minY = y;
			}
			if (y > maxY) {
				maxY = y;
			}
		}

		// Loop through all vertices and normalize the Y-values
		for (int i = 1; i < FractalVertices.size(); i += 3) {
			float y = FractalVertices[i];

			// Normalize the Y-value to fall within the [0, 1] range based on the original data range
			FractalVertices[i] = (y - minY) / (maxY - minY);
		}

		// Loop through the specified number of iterations
		for (int iteration = 0; iteration < iIterations; iteration++) {
			
			
			float iterRatio = ((float)iteration / (float)iIterations);
			iMaxDelta= iHeight - iterRatio * deltaHeight;	



			// Randomly select two points (iRandX1, iRandZ1) and (iRandX2, iRandZ2)
			float iRandX1 = rand() % this->size;  // Width is the width of your terrain grid
			float iRandZ1 = rand() % this->size; // Length is the length of your terrain grid
			float iRandX2 = rand() % this->size;
			float iRandZ2 = rand() % this->size;

			// Loop through all vertices (grouped as XYZ elements)
			for (int i = 0; i < FractalVertices.size(); i += 3) {
				float xPos = FractalVertices[i];
				float yPos = FractalVertices[i + 1];
				float zPos = FractalVertices[i + 2];

				// Calculate the side of the fault line based on cross product
				float side = (iRandX2 - iRandX1) * (zPos - iRandZ1) - (xPos - iRandX1) * (iRandZ2 - iRandZ1);

				// If the point is on one side of the line, adjust its height (Y-axis)
				if (side > 0) {
					FractalVertices[i + 1] += iMaxDelta;
				}
			}

			// Adjust iHeight for the next iteration if desired
			iMaxDelta *= 0.95;
		}

	
		
	return FractalVertices;
}

	void Terrain::terrainEditUI(GLFWwindow* window)
	{
		//GLint drawDistanceLocation = glGetUniformLocation(*defaultShaderProgramPtr, "drawDistance");

		ImGui::Begin("Edit Terrain");


		if (ImGui::RadioButton("Point Brush", currentBrushType == POINT_BRUSH)) {
			currentBrushType = POINT_BRUSH;
			currentTool = Tool::NONE;
		}
		if (ImGui::RadioButton("Circle Brush", currentBrushType == CIRCLE_BRUSH)) {
			currentBrushType = CIRCLE_BRUSH;
			currentTool= Tool::TERRAIN_SCULPT;
		}
		if (ImGui::RadioButton("Square Brush", currentBrushType == SQUARE_BRUSH)) {
			currentBrushType = SQUARE_BRUSH;
			currentTool = Tool::TERRAIN_SCULPT;
		}

		if (ImGui::RadioButton("Flatten Brush", currentBrushType == FLATTEN_BRUSH))
{
			currentBrushType = FLATTEN_BRUSH;
			currentTool = Tool::TERRAIN_SCULPT;
		}
		ImGui::Checkbox("Relative Height", &boolRelativeHeight);
		ImGui::InputInt("Brush Size", &brushSize);

		// Optional: Clamp the brush size to ensure it stays within a reasonable range
		if (brushSize < 1) brushSize = 1; // Minimum brush size
		//if (brushSize > 100) brushSize = 100; // Maximum brush size (adjust as needed)
		// Set brush size
		if (currentBrushType == CIRCLE_BRUSH) {
			ImGui::SliderFloat("Circle Radius", &brushRadius, 1.0f, 50.0f);
		}
		else if (currentBrushType == SQUARE_BRUSH) {
			ImGui::SliderInt("Square Size", &brushSize, 1, 20);
		}
		ImGui::SliderInt("Brush Impact", &brushImpact, -100,100);


		if (ImGui::Selectable("Terrain Tool On/Off", boolTerrainToolSwitch)) {
			std::cout << "\nTerrain tool switched";
			if (terrainPickingSwitch == 0)
			{
				terrainPickingSwitch = 1;
				//terrainRenderToFBO();
				;//render to fbo whilst editing. also to world view 
			}
			else if (terrainPickingSwitch == 1)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				terrainPickingSwitch = 0;
				GLint editModeLocation = glGetUniformLocation(*this->shaderPtr, "terrainEditMode");
				glUniform1i(editModeLocation, terrainPickingSwitch);
				
		}
			//oops.
			std::cout << "\nTerrain int value: " << terrainPickingSwitch;
			
		}
		if (ImGui::Button("Recreate Phyics Terrain"))
		{

			GLuint error;
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error at start of create terrain mesh function: " << error << std::endl;
			}
			int detailMapWidth, detailMapHeight, detailMapChannels;
			unsigned char* detailMapData = stbi_load("pmossheight.jpg", &detailMapWidth, &detailMapHeight, &detailMapChannels, 0);

			// Generate and bind the texture
			glGenTextures(1, &detailMapTexture);
			glBindTexture(GL_TEXTURE_2D, detailMapTexture);

			if (detailMapData) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, detailMapWidth, detailMapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, detailMapData);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else {
				// Handle texture loading error
				std::cerr << "Failed to load detail map" << std::endl;
			}


			////mudHeight = loadTexture("mudheight.jpg");
			//glGenTextures(1, &mudMapTexture);
			//glBindTexture(GL_TEXTURE_2D, mudMapTexture);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//int mudMapWidth, mudMapHeight, mudMapChannels;
			//unsigned char* mudMapData = stbi_load("grass2.jpg", &mudMapWidth, &mudMapHeight, &mudMapChannels, 0);

			//if (mudMapData) {
			//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mudMapWidth, mudMapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mudMapData);
			//	glGenerateMipmap(GL_TEXTURE_2D);
			//}
			//else {
			//	// Handle texture loading error
			//	std::cerr << "Failed to load mud map" << std::endl;
			//}
			//glGenTextures(1, &mudHeight);
			//glBindTexture(GL_TEXTURE_2D, mudHeight);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//int mudHeightWidth, mudHeight, mudHeightChannels;
			//unsigned char* mudHeightData = stbi_load("mudheight.jpg", &mudHeightWidth, &mudHeight, &mudHeightChannels, 0);

			//if (mudHeightData) {
			//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mudHeightWidth, mudHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mudHeightData);
			//	glGenerateMipmap(GL_TEXTURE_2D);
			//}
			//else {
			//	// Handle texture loading error
			//	std::cerr << "Failed to load mud h map" << std::endl;
			//}

			//glGenTextures(1, &mudNormals);
			//glBindTexture(GL_TEXTURE_2D, mudNormals);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//int mudNormalWidth, mudNormalHeight, mudNormalChannels;
			//unsigned char* mudNormalData = stbi_load("mudNormals.jpg", &mudNormalWidth, &mudNormalHeight, &mudNormalChannels, 0);

			//if (mudHeightData) {
			//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mudNormalWidth, mudNormalHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mudNormalData);
			//	glGenerateMipmap(GL_TEXTURE_2D);
			//}
			//else {
			//	// Handle texture loading error
			//	std::cerr << "Failed to load mud h map" << std::endl;
			//}

			// Free the image data
			stbi_image_free(detailMapData);
		//	vertices.clear();
			normals.clear();
			indices.clear();
			terrainInfoVector.clear();
			verticesID.clear();//Unique IDs for terrain picking


			std::vector<GLfloat> colours;
			//std::vector<glm::vec3> normals;

			  // Create a vector to store normals for each vertex.
			const float minThreshold = 0.1f;    // Adjust these thresholds as needed
			const float mountainThreshold = 0.7f;
			const float snowThreshold = 0.9f;
			const glm::vec3 grassyColor = glm::vec3(0.1f, 0.8f, 0.2f);  // Green
			const glm::vec3 mountainColor = glm::vec3(0.5f, 0.4f, 0.3f);  // Dark brown for lower areas
			const glm::vec3 snowColor = glm::vec3(1.0f, 1.0f, 1.0f);     // White for snow


			float scaleX = 1.0f; // Scale factor along the X-axis
			float scaleY = 1.0f; // Scale factor along the Y-axis
			float scaleZ = 1.0f; // Scale factor along the Z-axis
			glm::vec3 lowerColor(0.2, 0.2, 0.2);
			glm::vec3 upperColor(0.8, 0.8, 0.8);
			float minHeight = 0.1, maxHeight = 0.2;
			// Generate terrain vertices with scaling
			TerrainInfo terrainEntry;

			//for (int z = 0; z < size; ++z) {
			//	for (int x = 0; x < size; ++x) {
			//		float xPos = static_cast<float>(x) * scaleX;
			//		float zPos = static_cast<float>(z) * scaleZ;
			//		float yPos = this->heightmapData.heights[x + z * this->size];// Adjust terrain height function if needed

			//		// Store the scaled vertex position
			//		vertices.push_back(xPos);
			//		vertices.push_back(yPos); // Apply Y-axis scaling
			//		vertices.push_back(zPos);
			//		float id = xPos + yPos;
			//		verticesID.push_back(id);//This will createa unique number since these values
			//		//increase, then we just need the range
			//		//Below we are creating one 'terrainEntry' for each vertices of terrain this allows to then edit these vertices
			//		//and allocate some of them as water etc. This data is passed to the vertex shader in to a 'location attrib buffer'
			//		//and only one entry per 3 vertices is required, since 3 vertices corrosponds to one height, or one x,y,z
			//		terrainInfoVector.push_back(terrainEntry);
			//		if (yPos < minHeight)
			//			minHeight = yPos;
			//		if (yPos > maxHeight)
			//			maxHeight = yPos;

			//		float grayColor = yPos; // You can modify this calculation for better color mapping

			//		// Calculate a normalized height value between 0 and 1
			//		float normalizedHeight = (yPos - minHeight) / (maxHeight - minHeight);

			//		// Interpolate between lowerColor and upperColor based on normalizedHeight
			//		glm::vec3 finalColor = (1.0f - normalizedHeight) * lowerColor + normalizedHeight * upperColor;

			//		// Store the color for this vertex
			//		colours.push_back(finalColor.r);
			//		colours.push_back(finalColor.g);
			//		colours.push_back(finalColor.b);

			//		// Normalize the grayscale color to the range [0, 1]
			//		//grayColor = (grayColor - minThreshold) / (snowThreshold - minThreshold);



			//	}
			//}

			// Find the minimum and maximum values
			float minID = 0;
			float maxID = 1;



			fractalTerrain();
			voxelateTerrain();
			firSmoothTerrain();
			mountainsTerrain();
			std::cout << "min Height was: " << minHeight;
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



			//glm::vec3 center(0.0f, 0.0f, 0.0f); // Initialize a vector to store the center
			//for (size_t i = 0; i < vertices.size(); i += 3) {
			//	center.x += vertices[i];
			//	center.y += vertices[i + 1];
			//	center.z += vertices[i + 2];
			//}
			//center /= (vertices.size() / 3); // Divide by the number of vertices to get the average position

			//// Calculate the translation needed to move the center to the local origin   
			//float translationX = -center.x;
			//float translationY = -center.y;
			//float translationZ = -center.z;

			//// Apply the translation to all the vertices
			//for (size_t i = 0; i < vertices.size(); i += 3) {
			//	vertices[i] += translationX;
			//	vertices[i + 1] += translationY;
			//	vertices[i + 2] += translationZ;
			//}


			glm::vec3 transformedVertex = glm::vec3(modelMatrix * glm::vec4(vertices[0], vertices[1], vertices[2], 1.0f));

			terrainBottom = transformedVertex.y;
			terrainTop = transformedVertex.y;

			for (int i = 0; i + 2 < vertices.size(); i += 3)
			{
				// Transform the vertex using the model matrix
				transformedVertex = modelMatrix * glm::vec4(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);

				if (transformedVertex.y < terrainBottom)
					terrainBottom = transformedVertex.y;
				else if (transformedVertex.y > terrainTop)
					terrainTop = transformedVertex.y;
			}

			//We need to loop through the vertices, each X<Y<Z and turn it into a unique value that is noralised (between 0 and 1,
			//and pass that number to the colour

			for (int i = 0; i < verticesID.size(); i += 3) {
				//	std::cout << "\nBefore normalisation: " << i;//

				//float minValue std::min_element(verticesID.begin(), verticesID.end());
				//float maxValue std::max_element(verticesID.begin(), verticesID.end());

				//std::transform(verticesID.begin(), verticesID.end(), verticesID.begin(),
				//	[&](float value) { return (value - minValue) / (maxValue - minValue); });
				////	std::cout << "\tValue: " << verticesID[i];

			}
			createUVs();



			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error at start of terrain mesh binding : " << error << std::endl;
			}
	while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error at start of create terrain mesh function: " << error << std::endl;
			}

			//Vertice IDs sent in as floats, normalised. Phys.H file has the code for the shaders.
			
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
			//terrainMesh = new btTriangleMesh();

			// Populate the btTriangleMesh with your vertex and index data
			// Create the btHeightfieldTerrainShape using your heightmap data
			terrainShapePtr = new btHeightfieldTerrainShape(
				size,                 // Width of the heightmap (number of points along X axis)
				size,                 // Length of the heightmap (number of points along Z axis)
				&heightmapData.heights,      // Pointer to your heightmap height data
				1.0f,                           // Height scale (usually 1.0f, adjust as needed)
				0,                      // Minimum height value
				12,                      // Maximum height value
				1,                              // Up axis (Y-axis in this case, so it's 1)
				PHY_FLOAT,                      // Data type of height (here, float)
				false                           // FlipQuadEdges (usually false)
			);
			logger.AddLog(std::to_string(minHeight), ImGuiLogger::LogType::Physics);
			logger.AddLog(std::to_string(minHeight), ImGuiLogger::LogType::Physics);
			// Optionally set scaling if needed (adjust this as per your terrain size)
			btVector3 localScaling(1.0, 1.0f, 1.0);  // Scale for X, Z axes (Y scale is managed by heightmap heights)
			terrainShapePtr->setLocalScaling(localScaling);

			// Now create the rigid body for the terrain
			btTransform startTransform;
			startTransform.setIdentity();
			startTransform.setOrigin(btVector3(0.0, 0.0, 0.0));  // Position the terrain at origin

			btDefaultMotionState* terrainMotionState = new btDefaultMotionState(startTransform);

			// Create the rigid body construction info
			btRigidBody::btRigidBodyConstructionInfo terrainRigidBodyCI(
				0.0f,  // Mass of 0, because it's static
				terrainMotionState,
				terrainShapePtr,
				btVector3(0, 0, 0)  // Local inertia is zero for static objects
			);

			// Create the terrain rigid body
			terrainMeshRigidBody = new btRigidBody(terrainRigidBodyCI);
			dynamicsWorldUniversalPtr->addRigidBody(terrainMeshRigidBody);

			if (terrainMeshRigidBody == nullptr)
			{
				std::cout << "\nRemoved rigid body from dynamics world to generate new terrain and RB";
				dynamicsWorldUniversalPtr->addRigidBody(terrainMeshRigidBody);
			}
		}
		

		

		ImGui::SliderFloat("Draw Distance", &drawDistance, 10.0f, 4000.0f);
		//glUniform1f(drawDistanceLocation, drawDistance);

		ImGui::End();
		drawTerrainHistory();
		pickTerrain(window);

	}

	void Terrain::initPickingBuffer()
	{
		//generate picking buffers and fbuffers - used in terrain selection functions
		glGenFramebuffers(1, &terrainPickFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, terrainPickFBO);


		glGenTextures(1, &terrainPickTexture);
		glBindTexture(GL_TEXTURE_2D, terrainPickTexture);
		//Set texture wrapping parameters and size - remember to resize on
		//screen size change in main.cpp or elsewhere
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainPickTexture, 0);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error after setting up Terrain FBO textures: " << error << std::endl;

		}
		glGenTextures(1, &terrainPickPickingBuffer);
		glBindTexture(GL_TEXTURE_2D, terrainPickPickingBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, terrainPickPickingBuffer, 0);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error after setting up Terrain FBO textures: " << error << std::endl;

		}

	/*	glGenRenderbuffers(1, &terrainPickDepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, terrainPickDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, terrainPickDepthBuffer);*/

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "\nFrame buffer not complete";
		}
		else std::cout << "\nTerrain buffer complete";

		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "\nend initPickingBuffer function";
	
	}

	void Terrain::terrainRenderToFBO()
	{
		//issue may be the terrain shader has no uniform data
		GLenum error;
		GLint editModeLocation = glGetUniformLocation(*this->shaderPtr, "terrainEditMode");
		glUniform1i(editModeLocation, terrainPickingSwitch);
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error: " << error << std::endl;
		}
		//glUniform1i(*shaderPtr, terrainPickingSwitch);
		glBindFramebuffer(GL_FRAMEBUFFER, terrainPickFBO);
		//First bind the TARGET
		//FRAME BUFFER, this is the target that openGL will draw to.	
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, drawBuffers);	
		glViewport(200, 150, window_width, window_height);
		//glBindTexture(GL_TEXTURE_2D, terrainPickTexture);//not sure this is the problem
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error: " << error << std::endl;
		}
		if (heightmapData.heights.size() > 0) {
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
			
			
			//glBindTexture(GL_TEXTURE_2D, terrainPickTexture);//not sure this is the problem
			//glBindTexture(GL_TEXTURE_2D, terrainPickPickingBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);



			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}

			//loop through the Z axis of the terrain

				//loop through the X axis of the terrain
				//this is where the triangle strip is constructed
			glDrawElements(GL_TRIANGLES, (this->size) * (this->size) * 6, GL_UNSIGNED_INT, 0);
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}
			glDrawBuffer(GL_BACK);
			//	glBindVertexArray(VAO); // Bind the VAO
			glBindVertexArray(0); // Unbind the VAO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}


		//Screenshot of framebuffer for debugging
		if (ImGui::Button("Screenshot Terrain Buffer"))
		{
			GLubyte* pixels = new GLubyte[window_width * window_height * 4]; // 4 channels (RGBA)
			if (boolShowGLErrors) {
				GLenum error;
				while ((error = glGetError()) != GL_NO_ERROR) {
					std::cout << "\nopenGL Error at: BEFORE pixels for saving screenshot\NError creating pixels GLubyte" << error;
				}
			}
			std::cout << "\nScreenshot parameters at time of taking: " << window_width << ", " << window_height << std::endl;
			// Read the pixel data from the framebuffer
			glReadPixels(0, 0, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			if (boolShowGLErrors) {
				GLenum error;
				while ((error = glGetError()) != GL_NO_ERROR) {
					std::cout << "\nopenGL Error at: read pixels for saving screenshot" << error;
				}
			}
			// Unbind the framebuffer

			// Flip the pixel data vertically (OpenGL reads pixels from bottom to top)
			GLubyte* flippedPixels = new GLubyte[window_width * window_height * 4];
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}
			for (int y = 0; y < window_height; ++y) {
				for (int x = 0; x < window_width; ++x) {
					int sourceIndex = (window_height - y - 1) * window_width + x;
					int destIndex = y * window_width + x;

					memcpy(flippedPixels + destIndex * 4, pixels + sourceIndex * 4, 4);
				}
			}
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}
			// Save the pixel data as an image using stb_image_write
			stbi_write_png("terrainFBO.png", window_width, window_height, 4, flippedPixels, window_width * 4);

			// Clean up
			delete[] pixels;
			delete[] flippedPixels;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error: " << error << std::endl;
		}
	}

	void Terrain::pickTerrain(GLFWwindow* window)
	{

		ImGuiIO& io = ImGui::GetIO();

		// Check if ImGui is capturing the mouse input
		if (io.WantCaptureMouse)
		{
			// If ImGui is interacting with the mouse, don't process picking
			return;
		}


		if (terrainLMouseClicked && terrainPickingSwitch == 1) {//this means if true

			glBindFramebuffer(GL_FRAMEBUFFER, terrainPickFBO);
			double xpos, ypos;
			int width, height;

			glfwGetFramebufferSize(window, &width, &height);
			glfwGetCursorPos(window, &xpos, &ypos);
			// Set viewport
			glViewport(200, 150, width, height);


			// Bind texture for GL_COLOR_ATTACHMENT0 (assuming colorTexture is defined somewhere)
			glBindTexture(GL_TEXTURE_2D, terrainPickTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainPickTexture, 0);

			// Check framebuffer completeness
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				// Handle error, framebuffer is not complete
				std::cout << "Framebuffer not complete!" << std::endl;
			}


			glBindTexture(GL_TEXTURE_2D, terrainPickPickingBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, terrainPickPickingBuffer, 0);
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error after setting up Terrain FBO textures: " << error << std::endl;

			}
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "Framebuffer is not complete after resize!" << std::endl;
			}


			glBindFramebuffer(GL_FRAMEBUFFER, terrainPickFBO);


			terrainLMouseClicked = false;

			GLuint pixelColor[4];
			//GLfloat pickpixelColor[2]{};

			GLubyte pickpixelColor2[4];
			//std::cout << pickpixelColor;


		//std::cout << "\nglfw sees x: " << xpos << ", y: " << ypos << std::endl;
			int glX = static_cast<int>(xpos);
			int glY = height - static_cast<int>(ypos) - 1;


			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				// Handle framebuffer completeness issue
				std::cout << "Framebuffer not complete! ('pickTerrain' function" << std::endl;
			}
			std::cout << "\nX: " << xpos;
			GLint currentFramebuffer;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);
			GLenum error;

			glReadBuffer(GL_COLOR_ATTACHMENT1);
			//glReadPixels(xpos, ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixelColor);
			glReadPixels(glX, glY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pickpixelColor2);
			rgbSelected[0] = pickpixelColor2[0];
			rgbSelected[1] = pickpixelColor2[1];//1 should be y if picking XYZ in the frag shader in 'phys.h'
			rgbSelected[2] = pickpixelColor2[2];
			rgbSelected[3] = pickpixelColor2[3];

			//glReadPixels(xpos, ypos, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pickpixelColor);
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error: " << error << std::endl;



			}
			//std::cout << "\nMouse co ordinates, X:" << glX << " Y:" << glY << std::endl;
			std::cout << "Pick data: " << pickpixelColor2;
			std::cout << "\n" << static_cast<int>(pickpixelColor2[0]) << "," << static_cast<float>(pickpixelColor2[1]);// << ", " << static_cast<int>(pixelColor[2]) << ", " << static_cast<int>(pixelColor[3]);
			int pushback = static_cast<int>(pickpixelColor2[0]);
			int pushback1 = static_cast<int>(pickpixelColor2[1]);
			int pushback2 = static_cast<int>(pickpixelColor2[2]);

			terrainPickedLocationsVector.push_back(pushback);
			terrainPickedLocationsVector.push_back(pushback1);

			terrainPickedLocationsVector.push_back(pushback2);

			currentTerrainClickedRGB.clear();//Clear the vector before adding latest picked locations (X,Y,Z)
			currentTerrainClickedRGB.push_back(pushback);//Add red green and blue to vector
			currentTerrainClickedRGB.push_back(pushback1);
			currentTerrainClickedRGB.push_back(pushback2);



			int r = currentTerrainClickedRGB[0]; // Red channel (encoded X)
			int g = currentTerrainClickedRGB[1]; // Green channel (could be ignored or hold another data)
			int b = currentTerrainClickedRGB[2]; // Blue channel (encoded Z)

			// Decode the RGB values back to the original X and Z indices in the grid.
			int X = static_cast<int>((r / 254.0f) * 127.0f);
			int Z = static_cast<int>((b / 254.0f) * 127.0f);

			// Ensure that the indices are within valid range.
			X = std::min(std::max(X, 0), size - 1);
			Z = std::min(std::max(Z, 0), size - 1);

			int index = (Z * size + X) * 3; // Multiply by 3 because each vertex has X, Y, and Z

			// Access the Y (height) value.
			float heightbkup = vertices[index + 1]; // Y value is at index + 1

			// Now you can modify the height or do other operations.
			float newHeight = 50.0f; // Example new height value
			//remove comment to edit a single point. 
			// vertices[index + 1] = newHeight;


			//We need an if statement, or switch, to test the type of brush selected, and choose corresponding function.

			switch (currentBrushType) {
			case CIRCLE_BRUSH:

				toolsCircleBrush(vertices, X, Z, size, brushRadius, brushImpact);
				break;

			case SQUARE_BRUSH:
				toolsSquareBrush(vertices, X, Z, size, brushSize, brushImpact);
				break;

			case FLATTEN_BRUSH:
				toolsFlattenBrush(vertices, X, Z, size, brushSize);
			}

			GLint brushSizeLoc = glGetUniformLocation(*shaderPtr, "brushSize");  // Get uniform location
			glUseProgram(*shaderPtr);  // Activate the shader program
			glUniform1i(brushSizeLoc, brushSize);  // Send brushSize to the shader
			//	std::vector<GLfloat>normalizedRGB;
			/*	for (int value : currentTerrainClickedRGB) {
					float normalizedValue = value / 255.0f;
					normalizedRGB.push_back(normalizedValue);
				}*/
				//std::swap(currentTerrainClickedRGB, normalizedRGB);

				//Send latest vector data to the graphics shader from the cpu.
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, TerrainClickedRGB_VBO);
			glBufferData(GL_ARRAY_BUFFER, currentTerrainClickedRGB.size() * sizeof(GLfloat), currentTerrainClickedRGB.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			if (!currentBrushType == BrushType::POINT_BRUSH)
			{
				terrainBackup();
			}

			glBindVertexArray(VAO);


			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(1);
			// Specify vertex attribute pointer for colors (location = 1)
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);


			rgbSelectedTerrain[0] = pixelColor[0];
			rgbSelectedTerrain[1] = pixelColor[1];
			rgbSelectedTerrain[2] = pixelColor[2];
			rgbSelectedTerrain[3] = pixelColor[3];
			std::cout << "\nRGB seleted: " << rgbSelectedTerrain[0] << ", " << rgbSelectedTerrain[1] << ", " << rgbSelectedTerrain[2];
			glm::vec3 colour;
			colour.r = rgbSelectedTerrain[0];

			colour.g = rgbSelectedTerrain[1];
			colour.b = rgbSelectedTerrain[2];
			//int vertEdit = 
			//vertices[]
			//pickpixelColor = 0;
			//glReadBuffer(GL_COLOR_ATTACHMENT1);
			//GLuint* pixelData = new GLuint[width * height];
			//glReadPixels(0, 0, width, height, GL_RGBA_INTEGER, GL_UNSIGNED_INT, pixelData);

		/*	glReadPixels(xpos, ypos, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pickpixelColor);

			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "\nOpenGL Error at engine terrain picking / read pixels: " << error << std::endl;

			}
			std::cout << "Color Attachment 1: " << pickpixelColor;

			std::cout << "Pause";*/

		}
		//	}
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);	
		if (terrainPickingSwitch && boolToolResized)
		{		GLint brushSizeLoc = glGetUniformLocation(*shaderPtr, "brushSize");  // Get uniform location
		glUseProgram(*shaderPtr);  // Activate the shader program
		glUniform1i(brushSizeLoc, brushSize);  // Send brushSize to the shader

		boolToolResized = false;
		}

		for (auto value : terrainPickedLocationsVector) {
		
			ImGui::Text("%.2f", value);
		}

		terrainLMouseClicked = false;
		
	}

	void Terrain::toolsCircleBrush(std::vector<float>& vertices, int X, int Z, int size, float radius, float newHeight)
	{
	
		{
			// Converts the radius in world units to a radius in terms of the vertex grid
			int radiusInVertices = static_cast<int>(radius);

			// Iterate over all vertices within the square bounding box defined by the brush's radius
			for (int dz = -radiusInVertices; dz <= radiusInVertices; ++dz) {
				for (int dx = -radiusInVertices; dx <= radiusInVertices; ++dx) {
					// Calculate the position of the current vertex in the grid
					int newX = X + dx;
					int newZ = Z + dz;

					// Ensure that the current vertex is within the grid boundaries
					if (newX >= 0 && newX < size && newZ >= 0 && newZ < size) {

						// Calculate the squared distance from the brush center to the current vertex
						float distanceSquared = dx * dx + dz * dz;

						// Check if the current vertex is within the circular brush's radius
						if (distanceSquared <= radiusInVertices * radiusInVertices) {

							// Calculate the influence of the brush based on the distance from the center
							float distance = sqrt(distanceSquared); // Actual distance from the center
							float falloff = 2.0f - (distance / radiusInVertices); // Linear falloff

							// Optionally modify the falloff to be smoother (quadratic)
							// float falloff = (1.0f - (distance / radiusInVertices)) * (1.0f - (distance / radiusInVertices));

							// Calculate the index of the current vertex in the vertex array
							int index = (newZ * size + newX) * 3;

							// Apply the height modification with the falloff effect
							if (boolRelativeHeight) {//relative heught switch
								// Adjust height relative to the current height
								vertices[index + 1] += newHeight * falloff;
							}
							else {
								// Set height absolutely based on falloff
								vertices[index + 1] = newHeight * falloff + vertices[index + 1] * (1.0f - falloff);
							}
						}
					}
				}
			}
		}
	}

	void Terrain::toolsSquareBrush(std::vector<float>& vertices, int X, int Z, int size, float halfSideLength, float newHeight)
	{
	
		for (int dz = -halfSideLength; dz <= halfSideLength; ++dz) {
			for (int dx = -halfSideLength; dx <= halfSideLength; ++dx) {
				int newX = X + dx;
				int newZ = Z + dz;

				// Ensure we're within bounds
				if (newX >= 0 && newX < size && newZ >= 0 && newZ < size) {
					int index = (newZ * size + newX) * 3;
					vertices[index + 1] += newHeight; // Modify the height
				}
			}
		}

	}

	void Terrain::toolsFlattenBrush(std::vector<float>& vertices, int X, int Z, int size, float radius)
	{
		// Converts the radius in world units to a radius in terms of the vertex grid
		int radiusInVertices = static_cast<int>(radius);

		// Step 1: Compute the average height within the brush area
		float totalHeight = 0.0f;
		int count = 0;

		for (int dz = -radiusInVertices; dz <= radiusInVertices; ++dz) {
			for (int dx = -radiusInVertices; dx <= radiusInVertices; ++dx) {
				int newX = X + dx;
				int newZ = Z + dz;

				// Ensure the vertex is within the bounds of the grid
				if (newX >= 0 && newX < size && newZ >= 0 && newZ < size) {
					float distanceSquared = dx * dx + dz * dz;

					// Check if the vertex is within the circular brush radius
					if (distanceSquared <= radiusInVertices * radiusInVertices) {
						int index = (newZ * size + newX) * 3;
						totalHeight += vertices[index + 1];  // Sum the heights
						count++;
					}
				}
			}
		}

		// Compute the average height of the vertices within the brush radius
		float averageHeight = (count > 0) ? (totalHeight / count) : 0.0f;

		// Step 2: Flatten the area to the average height with falloff
		for (int dz = -radiusInVertices; dz <= radiusInVertices; ++dz) {
			for (int dx = -radiusInVertices; dx <= radiusInVertices; ++dx) {
				int newX = X + dx;
				int newZ = Z + dz;

				// Ensure the vertex is within the bounds of the grid
				if (newX >= 0 && newX < size && newZ >= 0 && newZ < size) {
					float distanceSquared = dx * dx + dz * dz;

					// Check if the vertex is within the circular brush radius
					if (distanceSquared <= radiusInVertices * radiusInVertices) {
						// Calculate distance and falloff effect
						float distance = sqrt(distanceSquared);
						float falloff = 1.0f - (distance / radiusInVertices);  // Linear falloff

						// Index into the vertex array (assume y-coordinate is at index + 1)
						int index = (newZ * size + newX) * 3;

						// Smoothly blend the current height towards the average height
						vertices[index + 1] = averageHeight * falloff + vertices[index + 1] * (1.0f - falloff);
					}
				}
			}
		}
	}

	void Terrain::terrainBackup()
	{
		verticesHistory.push_back(vertices);
	}

	void Terrain::drawTerrainHistory()
	
	{
		ImGui::Begin("Terrain History Navigator");

		ImGui::Text("History size: %d", static_cast<int>(verticesHistory.size()));
		ImGui::Text("Viewing index: %d", currentHistoryIndex == -1 ? static_cast<int>(verticesHistory.size()) - 1 : currentHistoryIndex);

		// Fix for 'Previous' button
		if (ImGui::Button("Previous") && currentHistoryIndex > 0) {
			std::swap(vertices, verticesHistory[--currentHistoryIndex]);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		}

		ImGui::SameLine();

		// Fix for 'Next' button
		if (ImGui::Button("Next") && currentHistoryIndex < static_cast<int>(verticesHistory.size()) - 1) {
			std::swap(vertices, verticesHistory[++currentHistoryIndex]);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
		}

		// Fix for 'Reset to Active Terrain'
		if (ImGui::Button("Reset to Active Terrain") && currentHistoryIndex != -1) {
			// Reset to the latest version in history
			currentHistoryIndex = static_cast<int>(verticesHistory.size()) - 1;
			std::swap(vertices, verticesHistory[currentHistoryIndex]);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
		}

		ImGui::End();
	}


	
	
	


		//
		//// Define your TerrainData and HeightMapData structures
		//struct HeightMapData f
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
		//        layout (
		// 
		// 
		//  = 0) in vec3 position;
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
		//	glUniformMatrix4fv(glGetUniformLocaton(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		//
		//	glBindVertexArray(VAO);
		//	glDrawElements(GL_TRIANGLES, (terrain.size - 1) * (terrain.size - 1) * 6, GL_UNSIGNED_INT, 0);
		//	glBindVertexArray(0);
		//}}
	


void drawShaderManager() {
	glm::vec3 textPosition3D = glm::vec3(10.0f, 5.0f, -20.0f);

	// Apply transformations to position text in 3D space
	ImGui::SetNextWindowPos(ImVec2(textPosition3D.x, textPosition3D.y)); // Translate to 3D position

	ImGui::Begin("Shader Manager");
	static int selectedShaderIndex = -1;
	const char* comboLabel = "Select Shader";
	if (customShaders.size() > 0) {
		std::vector<const char*> shaderNames;
		for (const auto& shader : customShaders) {
			shaderNames.push_back(shader->shaderName.c_str());
		}
		ImGui::Combo(comboLabel, &selectedShaderIndex, shaderNames.data(), shaderNames.size());
	}

	// Display shader code for the selected shader
	if (selectedShaderIndex >= 0 && selectedShaderIndex < customShaders.size()) {
		ImGui::InputTextMultiline("Vertex Shader", customShaders[selectedShaderIndex]->vertexShaderCode.data(), customShaders[selectedShaderIndex]->vertexShaderCode.capacity());
		ImGui::InputTextMultiline("Fragment Shader", customShaders[selectedShaderIndex]->fragmentShaderCode.data(), customShaders[selectedShaderIndex]->fragmentShaderCode.capacity());

		// Display shader program ID
		ImGui::Text("Shader Program ID: %d", customShaders[selectedShaderIndex]->shaderProgramID);
	}

	
	ImGui::InputTextMultiline("Vertex Shader2", customVertexShaderCode.data(), customVertexShaderCode.capacity());
	ImGui::InputTextMultiline("Fragment Shader2", customFragmentShaderCode.data(), customFragmentShaderCode.capacity()); //, ImGuiInputTextFlags_AllowTabInput
	


	if (ImGui::Button("Add Custom Shader")) {
	
		customShader* newShader;
		newShader = new customShader;
		newShader->shaderName = "Shader" + std::to_string(customShaders.size() + 1);
		newShader->vertexShaderCode = std::string(customVertexShaderCode.c_str());
		newShader->fragmentShaderCode = std::string(customFragmentShaderCode.c_str());
		newShader->shaderProgramID = loadCustomShader(customVertexShaderCode, customFragmentShaderCode); // Assuming you have a function to load custom shaders.
		customShaders.push_back(newShader);

		customVertexShaderCode.clear(); // Clear the string for the next input.
		customFragmentShaderCode.clear(); // Clear the string for the next input.
	}

	ImGui::End();

}

GLuint loadCustomShader(const std::string& vertexShaderCode, const std::string& fragmentShaderCode) {
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexSource = vertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);

	GLint vertexShaderCompiled;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompiled);
	if (vertexShaderCompiled != GL_TRUE) {
		GLint logLength;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> vertexShaderError(logLength);
		glGetShaderInfoLog(vertexShader, logLength, &logLength, &vertexShaderError[0]);
		std::string customShaderErrorVert(vertexShaderError.begin(), vertexShaderError.end());
		std::cerr << "Vertex shader compilation failed: " << customShaderErrorVert << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragmentSource = fragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);

	GLint fragmentShaderCompiled;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompiled);
	if (fragmentShaderCompiled != GL_TRUE) {
		GLint logLength;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> fragmentShaderError(logLength);
		glGetShaderInfoLog(fragmentShader, logLength, &logLength, &fragmentShaderError[0]);
		std::string customShaderErrorFrag(fragmentShaderError.begin(), fragmentShaderError.end());
		std::cerr << "Fragment shader compilation failed: " << customShaderErrorFrag << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Error handling for shader linking can be added here.

	// Delete the shaders as they are now linked to the program.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}