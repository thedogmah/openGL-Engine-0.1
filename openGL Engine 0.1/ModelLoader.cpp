﻿#include "ModelLoader.h"
#include <filesystem>
#include "stb_image_write.h"
#include <stb/stb_image.h>
#include <unordered_map>
#include "Animation.h"
#include <assimp/scene.h>

namespace fs = std::filesystem;


struct TextureNew;
Sun sun  = {
		 1.0f,                     // Initial brightness
		 glm::vec3(1.0f, 0.95f, 0.9f),      // Initial diffuse color (soft sunlight color)
		 0.0f,                              // Initial radians time (0 to 2π)
		 glm::vec3(0.0f, -1.0f, 0.0f) };

Shader* modelLoader::shaderProgram = nullptr;

void modelLoader::clear() {

	vertices.clear();
	indices.clear();
	textures.clear();

}
void modelLoader::initAllMeshes(const aiScene* scene, const std::string& filename) {
	if (!scene) {
		logger.AddLog("Error: Scene is null.", ImGuiLogger::LogType::Assimp);
		throw std::runtime_error("Scene is null");
	}
	
	meshNewId++;
	auto model = std::make_unique<modelNew>();

	unsigned int numVerts = 0, numIndices = 0;

	// Count all vertices and indices from all meshes
	countVertAndIndices(scene, numVerts, numIndices);

	// Reserve space for vertices and indices
	reserveSpace(numVerts, numIndices);
	model->rootNode = scene->mRootNode;//###
	if (scene->mNumAnimations > 0)
	{

		model->meshIsAnimated = true;
	}
	model->loadedScene = const_cast<aiScene*>(scene); // Casting to non-const if necessary###

	modelNewVector.push_back(std::move(model)); // Move ownership of the model

	// Process the root node to initialize all meshes
	processNode(scene->mRootNode, scene, filename);
	logger.AddLog("Initialized All Meshes", ImGuiLogger::LogType::Assimp);

	//Scene has been added to the model vector (of structs)
	//Now we add a unique ID (meshNewID which we incremented) and the name of the root node
	modelNewVector.back()->modelID = meshNewId;
	modelNewVector.back()->name = scene->mRootNode->mName.C_Str();
}
bool modelLoader::initFromScene(const aiScene* scenePtr, const std::string& filename) {
	std::cout << "\init from Scene function start";
	if (!scenePtr || scenePtr->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scenePtr->mRootNode)
	{
			std::cout << "Error: Assimp could not load the model from: " << filename << std::endl;
		logger.AddLog("Assimple could not load the model", ImGuiLogger::LogType::Assimp);
		return false;
	}
	
	initAllMeshes(scenePtr, filename);


	if (scenePtr->mNumAnimations > 0) {
		loadAnimations(scenePtr);
		logger.AddLog("Animations loaded successfully", ImGuiLogger::LogType::Assimp);
	}
	else {

		logger.AddLog("No animations found for this model", ImGuiLogger::LogType::Assimp);
	}

	/*if (!initMaterials(scenePtr, filename)) {
		std::cout << "Error init materials from: " << filename << std::endl;
	}*/

	//for (auto& mesh : meshes) {
	//	//setupBuffersNew(mesh);
	//	
	//}
	
	createShaderProgram();
	
	std::cout << "\init from Scene function end";
	
	return true;
	

}

void modelLoader::countVertAndIndices(const aiScene* scenePtr, unsigned int& numVerts, unsigned int& numIndices) {
	numVerts = 0;
	numIndices = 0;

	std::cout << "\nCount Vertices function start";

	for (unsigned int i = 0; i < scenePtr->mNumMeshes; ++i) {

		const aiMesh* meshPtr = scenePtr->mMeshes[i];

		numVerts += meshPtr->mNumVertices;
		numIndices += meshPtr->mNumFaces * 3; //Each face in aiMesh is a triangle

	}
	std::stringstream logMessage;
	logMessage << "Counted Vertices: " << numVerts << ", Indices: " << numIndices;
	logger.AddLog(logMessage.str().c_str(), ImGuiLogger::LogType::Assimp);



}
void modelLoader::loadAnimations(const aiScene* scene) {
	for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
		const aiAnimation* animation = scene->mAnimations[i];
		
		// Store animation information (duration, channels, etc.)
		AnimationData animData;
		animData.duration = animation->mDuration;
		animData.ticksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f;

		for (unsigned int j = 0; j < animation->mNumChannels; j++) {
			aiNodeAnim* channel = animation->mChannels[j];

			// Extract position, rotation, and scaling keyframes for each bone
			BoneAnimation boneAnim;
			boneAnim.boneName = channel->mNodeName.data;

			// Process position keyframes
			for (unsigned int k = 0; k < channel->mNumPositionKeys; k++) {
				PositionKey posKey;
				posKey.time = channel->mPositionKeys[k].mTime;
				posKey.value = channel->mPositionKeys[k].mValue; // aiVector3D
				boneAnim.positions.push_back(posKey);
			}

			// Process rotation keyframes
			for (unsigned int k = 0; k < channel->mNumRotationKeys; k++) {
				RotationKey rotKey;
				rotKey.time = channel->mRotationKeys[k].mTime;
				rotKey.value = channel->mRotationKeys[k].mValue; // aiQuaternion
				boneAnim.rotations.push_back(rotKey);
			}

			// Process scaling keyframes
			for (unsigned int k = 0; k < channel->mNumScalingKeys; k++) {
				ScalingKey scaleKey;
				scaleKey.time = channel->mScalingKeys[k].mTime;
				scaleKey.value = channel->mScalingKeys[k].mValue; // aiVector3D
				boneAnim.scales.push_back(scaleKey);
			}

			// Add this bone animation to the animation data
			animData.boneAnimations.push_back(boneAnim);
		}

		// Add the full animation to your animations list
		animationsNew.push_back(animData);
	}
}

void modelLoader::reserveSpace(unsigned int numVertices, unsigned int numIndices)
{
	std::cout << "\nReserve space model function";
	vertices.reserve(numVertices);
	indices.reserve(numIndices);

	logger.AddLog("Reserved space for " + std::to_string(numVertices) + " vertices and " 
	+ std::to_string(numIndices) + " indices", ImGuiLogger::LogType::Assimp);

	std::cout << "\nreserve space function end";
}
void modelLoader::initSingleMesh(const aiMesh* meshPtr, const aiScene* scene, const std::string& filename) {
	if (!meshPtr || !scene) {
		logger.AddLog("Error: meshPtr or scene is null.", ImGuiLogger::LogType::Assimp);
		throw std::runtime_error("meshPtr or scene is null");
	}



	auto meshData = std::make_unique<MeshNew>();
	meshData->name = meshPtr->mName.C_Str();
	meshData->meshID = meshNewId;

	// Process vertices
	for (unsigned int i = 0; i < meshPtr->mNumVertices; ++i) {
		VertexNew vertex;
		vertex.Position = glm::vec3(meshPtr->mVertices[i].x, meshPtr->mVertices[i].y, meshPtr->mVertices[i].z);

		// Normals
		vertex.Normal = meshPtr->HasNormals()
			? glm::vec3(meshPtr->mNormals[i].x, meshPtr->mNormals[i].y, meshPtr->mNormals[i].z)
			: glm::vec3(0.0f, 0.0f, 0.0f); // Default normal

		// Texture Coordinates
		vertex.TexCoords = meshPtr->mTextureCoords[0]
			? glm::vec2(meshPtr->mTextureCoords[0][i].x, meshPtr->mTextureCoords[0][i].y)
			: glm::vec2(0.0f, 0.0f); // Default UV
		//HERE
		meshData->vertices.push_back(vertex);
	}
	std::unordered_map<std::string, int> boneMapping;

	// Iterate over all bones in the mesh
	for (unsigned int i = 0; i < meshPtr->mNumBones; ++i) {
		aiBone* bone = meshPtr->mBones[i];

		int boneIndex = 0;  // Variable to hold bone index
		std::string boneName(bone->mName.data);

		// Check if this bone is already in the bone mapping
		if (boneMapping.find(boneName) == boneMapping.end()) {
			// If boneName not found, assign a new index
			boneIndex = boneMapping.size();
			boneMapping[boneName] = boneIndex;
		}
		else {
			// If boneName found, use the existing index
			boneIndex = boneMapping[boneName];
		}

		// Loop through the vertices affected by this bone
		for (unsigned int j = 0; j < bone->mNumWeights; j++) {
			unsigned int vertexID = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;

			// Add the bone influence to the vertex
			assignBoneToVertex(meshData->vertices[vertexID], boneIndex, weight);
		}
	}


	// Process indices
	for (unsigned int i = 0; i < meshPtr->mNumFaces; ++i) {
		const aiFace& face = meshPtr->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			meshData->indices.push_back(face.mIndices[j]);
		}
	}

	// Insert the processed vertices and indices into the class member vectors
	vertices.insert(vertices.end(), meshData->vertices.begin(), meshData->vertices.end());
	indices.insert(indices.end(), meshData->indices.begin(), meshData->indices.end());


	// Process material and textures for this mesh
	if (meshPtr->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[meshPtr->mMaterialIndex];

		std::vector<TextureNew> textures;
		meshData->material = processMaterial(material, scene, filename, textures);  // Pass the textures vector

		// Now assign the textures to the meshData
		meshData->textures = textures;
	}

	// Log the successful initialization of the mesh
	logger.AddLog("Initialized single mesh with " + std::to_string(meshData->vertices.size()) +
		" vertices and " + std::to_string(meshData->indices.size()) + " indices",
		ImGuiLogger::LogType::Assimp);

	setupBuffersNew(*meshData);
	modelNewVector.back()->subMeshes.push_back(std::move(meshData));  // Move ownership of the mesh data
}

void modelLoader::assignBoneToVertex(VertexNew& vertex, int boneID, float weight) {
	int emptySlot = -1;
	float lowestWeight = 1.0f; // Start with maximum weight
	int lowestWeightIndex = -1;

	for (int i = 0; i < 4; ++i) {
		if (vertex.weights[i] == 0.0f) {
			emptySlot = i;
			break; // Found an empty slot, no need to continue
		}
		else if (vertex.weights[i] < lowestWeight) {
			lowestWeight = vertex.weights[i];
			lowestWeightIndex = i;
		}
	}

	if (emptySlot != -1) {
		// Found an empty slot, assign directly
		vertex.boneIDs[emptySlot] = boneID;
		vertex.weights[emptySlot] = weight;
	}
	else if (weight > lowestWeight) {
		// No empty slots, but this bone has more influence than the least influential one
		vertex.boneIDs[lowestWeightIndex] = boneID;
		vertex.weights[lowestWeightIndex] = weight;
	}
	else {
		// Bone influence is too low, discard (or consider blending)
		// You might want to log a warning here
	}

	// Normalize weights
	normalizeBoneWeights(vertex);
}

void modelLoader::normalizeBoneWeights(VertexNew& vertex) {
	float totalWeight = 0.0f;
	for (int i = 0; i < 4; ++i) {
		totalWeight += vertex.weights[i];
	}
	if (totalWeight > 0.0f) {
		for (int i = 0; i < 4; ++i) {
			vertex.weights[i] /= totalWeight;
		}
	}
}

void modelLoader::renderSubmesh(MeshNew& model, GLuint VAO2)
{
	std::cout << "\nRendering submesh function";
// Extract the model position from the existing model matrix
	
glm::vec3 modelPosition = glm::vec3(0.0,-0.13,-1.0); // Assuming subMatrixView is your existing view matrix

// Set the camera position relative to the model
glm::vec3 cameraOffset(submeshRenderCam[0], submeshRenderCam[1], submeshRenderCam[2]); // Move the camera further back
glm::vec3 cameraPosition = modelPosition + cameraOffset;

// Create the look-at view matrix
glm::mat4 lookAtViewMatrix = glm::lookAt(cameraPosition, modelPosition, glm::vec3(0.0f, 1.0f, 0.0f));

// Combine the existing view matrix with the lookAt transformation
glm::mat4 combinedViewMatrix = lookAtViewMatrix; // or subMatrixView * lookAtViewMatrix depending on your needs


	glUseProgram(this->shaderProgram->ID);
	glBindFramebuffer(GL_FRAMEBUFFER, submeshFBO);
	


	bool diffuseBound = false;
	bool specularBound = false;
	unsigned int placeholderTexture = CreatePlaceholderTexture();
	// Iterate over each texture in the sub-mesh
	for (const auto& texture : model.textures) {

		if (texture.type == "texture_diffuse") {
			// Bind diffuse texture to texture unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture.id);
			GLint diffuseSamplerLoc = glGetUniformLocation(this->shaderProgram->ID, "texture_diffuse");
			glUniform1i(diffuseSamplerLoc, 0);
			diffuseBound = true;

			// Log texture binding
		//	logger.AddLog("Bound diffuse texture ID: " + std::to_string(texture.id));
		}
		else if (texture.type == "texture_specular") {
			// Bind specular texture to texture unit 1
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture.id);
			GLint specularSamplerLoc = glGetUniformLocation(this->shaderProgram->ID, "texture_normal");
			glUniform1i(specularSamplerLoc, 1);
			specularBound = true;

			// Log texture binding
		//	logger.AddLog("Bound specular texture ID: " + std::to_string(texture.id));
		}
	}

	// Bind placeholder texture if no diffuse texture was bound
	if (!diffuseBound) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, placeholderTexture);
		//	logger.AddLog("Bound placeholder texture ID: " + std::to_string(placeholderTexture)); // Log placeholder
	}

	// Bind placeholder texture if no specular texture was bound
	if (!specularBound) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, placeholderTexture);
		//logger.AddLog("Bound placeholder texture ID: " + std::to_string(placeholderTexture)); // Log placeholder
	}
	// Set material properties
	Material& mat = model.material;
	GLint materialAmbientLoc = glGetUniformLocation(this->shaderProgram->ID, "material.ambient");
	GLint materialDiffuseLoc = glGetUniformLocation(this->shaderProgram->ID, "material.diffuseColor");
	GLint materialSpecularLoc = glGetUniformLocation(this->shaderProgram->ID, "material.specularColor");
	GLint materialShininessLoc = glGetUniformLocation(this->shaderProgram->ID, "material.shininess");
	glUniform1f(glGetUniformLocation(this->shaderProgram->ID, "material.transparency"), mat.transparency);

	if (materialAmbientLoc != -1) glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(mat.ambient));
	if (materialDiffuseLoc != -1) glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(mat.diffuseColor));
	if (materialSpecularLoc != -1) glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(mat.specularColor));
	if (materialShininessLoc != -1) glUniform1f(materialShininessLoc, mat.shininess);


	glBindVertexArray(VAO2);
	// Set sun properties
	GLint sunBrightnessLoc = glGetUniformLocation(this->shaderProgram->ID, "sunBrightness");
	glUniform1f(sunBrightnessLoc, sun.Brightness);

	GLint sunDiffuseColorLoc = glGetUniformLocation(this->shaderProgram->ID, "sunDiffuseColor");
	glUniform3f(sunDiffuseColorLoc, sun.DiffuseColor.r, sun.DiffuseColor.g, sun.DiffuseColor.b);

	GLint radiansTimeLoc = glGetUniformLocation(this->shaderProgram->ID, "radiansTime");
	glUniform1f(radiansTimeLoc, sun.radiansTime);
	GLint viewPosLoc = glGetUniformLocation(this->shaderProgram->ID, "viewPos");
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.getPosition()));

	// Set view matrix
	GLint viewLoc = glGetUniformLocation(this->shaderProgram->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(combinedViewMatrix));
	//glActiveTexture(GL_TEXTURE0);  // Activate texture unit 0
	//glBindTexture(GL_TEXTURE_2D, subMeshTexture);  // Bind the FBO texture
	// Set projection matrix
	
	GLint diffuseSamplerLoc = glGetUniformLocation(this->shaderProgram->ID, "texture_diffuse");
	if (diffuseSamplerLoc != -1) {
		glUniform1i(diffuseSamplerLoc, 0); // 0 refers to GL_TEXTURE0
	}
	else {
		// Handle the case where the uniform wasn't found
		std::cerr << "Could not find uniform 'diffuse_texture' in shader." << std::endl;
	}
	
	glViewport(0, 0, window_width, window_height);// Set the viewport to match FBO size
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear FBO buffers (color and depth)
	
	// Bind and render the submesh VAO
	
	glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, 0);
	
	
	// Unbind the FBO (return to default framebuffer)
	
		
	// Now display the FBO texture in ImGui
	ImGui::Begin("Submesh FBO Render");
	ImGui::Text("FBO Texture:");
	// Set the position and size of the image
	ImVec2 imageSize = ImVec2(window_width, window_height);
	ImVec2 imagePosition = ImGui::GetCursorScreenPos(); // Get the current cursor position

	// Draw the border using ImGui::GetWindowDrawList()
	// Set the border thickness and color
	float borderThickness = 2.0f;
	ImVec4 borderColor = ImVec4(1.0, 0.0, 0.0, 1.0);  // Red border (RGBA)

	// Get the draw list to draw custom shapes like the border
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Calculate the corner points for the border
	ImVec2 min = imagePosition;
	ImVec2 max = ImVec2(min.x + imageSize.x, min.y + imageSize.y);

	// Draw the border as a rectangle
	draw_list->AddRect(min, max, ImColor(borderColor), 0.0f, ImDrawFlags_None, borderThickness);
	MeshNew activeSubMesh;
	activeSubMesh = model;
	if (ImGui::Button("Next Submesh")) {
		// Increment the active submesh index
		activeSubMeshIndex++;

		// Wrap around if the index exceeds the number of submeshes
		if (activeSubMeshIndex >= sumMeshVAOSize) {
			activeSubMeshIndex = 0;  // Reset to the first submesh
		}

		// Update the currently rendered submesh and its VAO
		//activeSubMesh = model[activeSubMeshIndex].get();  // Access the new submesh
		VAOtoRender = activeSubMesh.VAO;  // Update the VAO to the new submesh
	}
	if (ImGui::Button("Previous Submesh")) {
		// Increment the active submesh index
		activeSubMeshIndex--;

		// Wrap around if the index exceeds the number of submeshes
		if (activeSubMeshIndex <= 0) {
			activeSubMeshIndex = 0;  // Reset to the first submesh
		}

		// Update the currently rendered submesh and its VAO
		//activeSubMesh = model[activeSubMeshIndex].get();  // Access the new submesh
		VAOtoRender = activeSubMesh.VAO;  // Update the VAO to the new submesh
	}

	// Display the rendered texture inside the border
	ImGui::Image((ImTextureID)(intptr_t)subMeshTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SliderFloat3("Camera XYZ", submeshRenderCam, -10.0f, 10.0f);  
	
	//ImGui::Image((ImTextureID)(intptr_t)subMeshTexture, ImVec2(700, 700), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0,1.0,1.0, 1.0));  // Display the rendered texture
	ImGui::End();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
}
	// Helper function to process material
Material modelLoader::processMaterial(aiMaterial* material, const aiScene* scene, const std::string& filename, std::vector<TextureNew>& textures) {
	std::string directory = fs::path(filename).parent_path().string();
	Material mat;

	// Extract diffuse color
	aiColor3D color(0.0f, 0.0f, 0.0f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		mat.diffuseColor = glm::vec3(color.r, color.g, color.b);

	// Extract specular color
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
		mat.specularColor = glm::vec3(color.r, color.g, color.b);

	// Extract ambient color
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color))
		mat.ambient = glm::vec3(color.r, color.g, color.b);

	// Extract emissive color
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, color))
		mat.emissiveColor = glm::vec3(color.r, color.g, color.b);

	// Extract shininess (Ns)
	float shininess = 0.0f;
	if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess))
		mat.shininess = shininess;
	else
		mat.shininess = 32.0f;  // Default shininess

	// Extract transparency (d or Tr)
	float transparency = 0.9f;
	if (AI_SUCCESS == material->Get(AI_MATKEY_OPACITY, transparency))
		mat.transparency = transparency;

	// Extract index of refraction (Ni)
	float ior = 1.0f;
	if (AI_SUCCESS == material->Get(AI_MATKEY_REFRACTI, ior))
		mat.indexOfRefraction = ior;

	// Extract illumination model (illum)
	int illum = 1;
	material->Get(AI_MATKEY_SHADING_MODEL, illum);
	mat.illuminationModel = illum;

	// Load textures for this material
	std::vector<TextureNew> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
	std::vector<TextureNew> specularMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_specular", directory);

	// Combine all textures
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	return mat;
}


// Remember to update `loadMaterialTextures` to be part of the class or make it static or free function 
// if it's not already designed to work in this context.

// Usage in your model loading process:


void modelLoader::setupBuffersNew(MeshNew& mesh)
{
	GLuint VAO1, VBO1, EBO1;

	// Generate and bind VAO
	glGenVertexArrays(1, &VAO1);
	glBindVertexArray(VAO1);
	logger.AddLog("VAO from setupBuffers: " + std::to_string(VAO1));

	// Generate and bind VBO
	glGenBuffers(1, &VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(VertexNew), mesh.vertices.data(), GL_STATIC_DRAW);

	// Generate and bind EBO
	glGenBuffers(1, &EBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

	// Set vertex attributes
	// Position attribute (layout location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (void*)offsetof(VertexNew, Position));
	glEnableVertexAttribArray(0);

	// Normal attribute (layout location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (void*)offsetof(VertexNew, Normal));
	glEnableVertexAttribArray(1);

	// Texture coordinate attribute (layout location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (void*)offsetof(VertexNew, TexCoords));
	glEnableVertexAttribArray(2);

	// Bone IDs attribute (layout location = 3)
	// Since boneIDs are integers, use `glVertexAttribIPointer` instead of `glVertexAttribPointer`
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexNew), (void*)offsetof(VertexNew, boneIDs));
	glEnableVertexAttribArray(3);

	// Bone Weights attribute (layout location = 4)
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (void*)offsetof(VertexNew, weights));
	glEnableVertexAttribArray(4);

	// Unbind the VAO (it's good practice)
	glBindVertexArray(0);

	// Store the VAO in the meshData for later use in rendering
	mesh.VAO = VAO1;
}

void modelLoader::displayAnimationDebugWindow(const std::vector<glm::mat4>& finalBoneTransforms, const std::unordered_map<std::string, aiMatrix4x4>& boneTransforms)
{ 
	if (!boolDisplayExtraAnimationData) return;

	ImGui::Begin("Animation Debug Information");

	// Display the size of the bone transform vectors
	ImGui::Text("finalBoneTransforms Size: %d", static_cast<int>(finalBoneTransforms.size()));
	ImGui::Text("boneTransforms Size: %d", static_cast<int>(boneTransforms.size()));

	// Display details for each element in finalBoneTransforms
	ImGui::Separator();
	ImGui::Text("Final Bone Transforms (GLM Matrices):");
	for (size_t i = 0; i < finalBoneTransforms.size(); ++i) {
		const glm::mat4& matrix = finalBoneTransforms[i];
		ImGui::Text("Bone %zu Matrix:", i);
		ImGui::Text("  %.2f %.2f %.2f %.2f", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
		ImGui::Text("  %.2f %.2f %.2f %.2f", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
		ImGui::Text("  %.2f %.2f %.2f %.2f", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
		ImGui::Text("  %.2f %.2f %.2f %.2f", matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
	}

	// Display details for each element in boneTransforms
	ImGui::Separator();
	ImGui::Text("Bone Transforms (Assimp Matrices):");
	for (const auto& [boneName, aiMatrix] : boneTransforms) {
		ImGui::Text("Bone: %s", boneName.c_str());
		ImGui::Text("  %.2f %.2f %.2f %.2f", aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4);
		ImGui::Text("  %.2f %.2f %.2f %.2f", aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4);
		ImGui::Text("  %.2f %.2f %.2f %.2f", aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4);
		ImGui::Text("  %.2f %.2f %.2f %.2f", aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4);
	}

	ImGui::End();
}


void modelLoader::Render(unsigned int shaderProgram, float deltaTime) {
	// Use the shader program passed to the function


	if (!boolDontRender) {
		//if (!modelNewVector.empty())
		showMaterialEditor(modelNewVector, activeModelIndex, activeSubMeshIndex);
	
		glUseProgram(this->shaderProgram->ID); // Use the shader program

		static float totalTime = 0.0f;
		totalTime = deltaTimeGlobal;
		//deltaTime = deltaTimeGlobal;
		// Set sun properties
		GLint sunBrightnessLoc = glGetUniformLocation(this->shaderProgram->ID, "sunBrightness");
		glUniform1f(sunBrightnessLoc, sun.Brightness);

		GLint sunDiffuseColorLoc = glGetUniformLocation(this->shaderProgram->ID, "sunDiffuseColor");
		glUniform3f(sunDiffuseColorLoc, sun.DiffuseColor.r, sun.DiffuseColor.g, sun.DiffuseColor.b);

		GLint radiansTimeLoc = glGetUniformLocation(this->shaderProgram->ID, "radiansTime");
		glUniform1f(radiansTimeLoc, sun.radiansTime);

		// ImGui Control Panel
		if (drawIMGUI) {
			ImGui::Begin("Control Panel");
			ImGui::SliderFloat("Ambient Light Intensity", &ambientLightIntensity, 0.0f, 10.0f);
			ImGui::SliderFloat3("Light Pos", glm::value_ptr(lightPosInput), -200, 200);
			ImGui::End();
		}
		// Set ambient light uniform
		GLint ambientLightLoc = glGetUniformLocation(this->shaderProgram->ID, "ambientLight");
		glUniform3f(ambientLightLoc, ambientLightIntensity, ambientLightIntensity, ambientLightIntensity);

		// Set light position
		GLint lightPosLoc = glGetUniformLocation(this->shaderProgram->ID, "lightPos");
		glUniform3f(lightPosLoc, lightPosInput.x, lightPosInput.y, lightPosInput.z);

		// Set view position
		GLint viewPosLoc = glGetUniformLocation(this->shaderProgram->ID, "viewPos");
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.getPosition()));

		// Set view matrix
		GLint viewLoc = glGetUniformLocation(this->shaderProgram->ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		glm::mat4 projections = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 30000.0f);
		// Set projection matrix
		GLint projLoc = glGetUniformLocation(this->shaderProgram->ID, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projections));

		// Placeholder texture
		unsigned int placeholderTexture = CreatePlaceholderTexture();
		unsigned int isAnimation = 0;
	
		for (const auto& modelPtr : modelNewVector) {
			float animationTime = totalTime; // Adjust for animation timing

			if (!modelPtr->rootNode || !animationControllerPtr) {
				isAnimation = 0;
				GLint isAnimationLoc = glGetUniformLocation(this->shaderProgram->ID, "isAnimation");
				glUniform1i(isAnimationLoc, isAnimation);
				continue;  // Skip the rest of the loop for this model
			}

			aiScene* scene = modelPtr->getScene();  // Assuming you have a method to get the scene

			// Check if the scene has animations and the model is animated
			bool hasValidAnimations = scene->mNumAnimations > 0 && scene->HasAnimations() ;
			bool hasBones = false;

			// Iterate through all meshes in the scene
			for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
				if (!scene->mMeshes[i]) {
					std::cerr << "Warning: Null mesh encountered at index " << i << "." << std::endl;
					continue; // Skip this mesh and proceed to the next
				}

				if (scene->mMeshes[i]->mNumBones > 0) {
					hasBones = true;
					break; // No need to check further
				}
			}

			if (hasBones) {
				isAnimation = 1;
				GLint isAnimationLoc = glGetUniformLocation(this->shaderProgram->ID, "isAnimation");
				glUniform1i(isAnimationLoc, isAnimation);

				// Proceed with animation setup as before
				animationControllerPtr->updateBoneTransformations(animationTime, modelPtr->rootNode, aiMatrix4x4());

				// Prepare bone matrices for the shader
				std::vector<glm::mat4> finalBoneTransforms;

				// Convert aiMatrix4x4 to glm::mat4 and store them
				for (const auto& boneTransform : animationControllerPtr->boneTransforms) {
					const aiMatrix4x4& aiBoneMatrix = boneTransform.second;
					glm::mat4 glmBoneMatrix = glm::transpose(glm::make_mat4(&aiBoneMatrix.a1)); // Convert Assimp matrix to GLM matrix
					finalBoneTransforms.push_back(glmBoneMatrix);
				}

				// Debug: Display bone transforms
				displayAnimationDebugWindow(finalBoneTransforms, animationControllerPtr->boneTransforms);

				// Upload bone matrices to the shader (assume MAX_BONES = 100 in shader)
				GLint boneMatricesLoc = glGetUniformLocation(this->shaderProgram->ID, "boneMatrices");
				glUniformMatrix4fv(boneMatricesLoc, finalBoneTransforms.size(), GL_FALSE, glm::value_ptr(finalBoneTransforms[0]));
			}
			else {
				// No bones, set isAnimation to 0
				isAnimation = 0;
				GLint isAnimationLoc = glGetUniformLocation(this->shaderProgram->ID, "isAnimation");
				glUniform1i(isAnimationLoc, isAnimation);

				// Log that no bones were found
				logger.AddLog("No Bones Found");
			}



			for (auto& subMesh : modelPtr->subMeshes) {
				// Set model matrix
				GLint modelLoc = glGetUniformLocation(this->shaderProgram->ID, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPtr->worldTransform));

				bool diffuseBound = false;
				bool specularBound = false;

				// Iterate over each texture in the sub-mesh
				for (const auto& texture : subMesh->textures) {
					if (texture.type == "texture_diffuse") {
						// Bind diffuse texture to texture unit 0
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, texture.id);
						GLint diffuseSamplerLoc = glGetUniformLocation(this->shaderProgram->ID, "texture_diffuse");
						glUniform1i(diffuseSamplerLoc, 0);
						diffuseBound = true;

						// Log texture binding
					//	logger.AddLog("Bound diffuse texture ID: " + std::to_string(texture.id));
					}
					else if (texture.type == "texture_specular") {
						// Bind specular texture to texture unit 1
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, texture.id);
						GLint specularSamplerLoc = glGetUniformLocation(this->shaderProgram->ID, "texture_normal");
						glUniform1i(specularSamplerLoc, 1);
						specularBound = true;

						// Log texture binding
					//	logger.AddLog("Bound specular texture ID: " + std::to_string(texture.id));
					}
				}

				// Bind placeholder texture if no diffuse texture was bound
				if (!diffuseBound) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, placeholderTexture);
					//	logger.AddLog("Bound placeholder texture ID: " + std::to_string(placeholderTexture)); // Log placeholder
				}

				// Bind placeholder texture if no specular texture was bound
				if (!specularBound) {
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, placeholderTexture);
					//logger.AddLog("Bound placeholder texture ID: " + std::to_string(placeholderTexture)); // Log placeholder
				}

				// Set material properties
				Material& mat = subMesh->material;
				GLint materialAmbientLoc = glGetUniformLocation(this->shaderProgram->ID, "material.ambient");
				GLint materialDiffuseLoc = glGetUniformLocation(this->shaderProgram->ID, "material.diffuseColor");
				GLint materialSpecularLoc = glGetUniformLocation(this->shaderProgram->ID, "material.specularColor");
				GLint materialShininessLoc = glGetUniformLocation(this->shaderProgram->ID, "material.shininess");
				glUniform1f(glGetUniformLocation(this->shaderProgram->ID, "material.transparency"), mat.transparency);

				if (materialAmbientLoc != -1) glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(mat.ambient));
				if (materialDiffuseLoc != -1) glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(mat.diffuseColor));
				if (materialSpecularLoc != -1) glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(mat.specularColor));
				if (materialShininessLoc != -1) glUniform1f(materialShininessLoc, mat.shininess);

				// Draw the subMesh
				glBindVertexArray(subMesh->VAO);
				std::string drawnstr = "Whole Submesh VAO DRawn is: " + std::to_string(subMesh->VAO);
				logger.AddLog(drawnstr, ImGuiLogger::LogType::Assimp);
				glDrawElements(GL_TRIANGLES, subMesh->indices.size(), GL_UNSIGNED_INT, 0);

				// Unbind VAO and textures
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);  // Unbind diffuse texture
				glBindTexture(GL_TEXTURE_2D, 1);  // Unbind specular texture
			}
		}

		// Restore default texture unit
		glActiveTexture(GL_TEXTURE0);
		if (boolRenderSubmesh) {
			renderSubmesh(*submeshRendered, VAOtoRender);

		}
	}




}




unsigned int modelLoader::loadTextureFromFile(const char* path, const std::string& directory)
{

	unsigned int textureID;
	glGenTextures(1, &textureID);
	logger.AddLog("Inside 'LoadTextureFromFile function", ImGuiLogger::LogType::Info);
	int width, height, nrComponents; 
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);


		if (data) {
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;	
		


		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,  data);
		glGenerateMipmap(GL_TEXTURE_2D);


		//set texture wrapping and filtering options
	// Bind the texture and set parameters
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Generate mipmaps after setting texture data
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		logger.AddLog("Texture loaded successfully: " + std::string(path), ImGuiLogger::LogType::Assimp);
		}
		else {
			std::cout << "Texture failed to load at path: " << path << std::endl;
			logger.AddLog("Texture failed to load at path: " + std::string(path), ImGuiLogger::LogType::Assimp);

			
		}
	return textureID;

}
void modelLoader::processNode(aiNode* node, const aiScene* scene, const std::string& filename) {
	// Process each mesh in the current node
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		initSingleMesh(mesh, scene, filename); // Initialize the mesh
	}

	// Recursively process each child node
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene, filename); // Process child nodes
	}
}



std::vector<TextureNew> modelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory) {
	std::vector<TextureNew> texturesLoaded;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString str;
		mat->GetTexture(type, i, &str);

		// Convert to std::string
		std::string texturePath = str.C_Str();

		// Find the last occurrence of '.' in the texture path
		size_t dotPos = texturePath.find_last_of('.');
		if (dotPos != std::string::npos) {
			// Keep only the filename and the extension, removing everything after
			texturePath = texturePath.substr(0, dotPos + 4); // Assuming extensions are 3 characters long, +1 for '.'
		}

		// Trim leading and trailing whitespaces from the path
		texturePath.erase(0, texturePath.find_first_not_of(" \t")); // Trim left
		texturePath.erase(texturePath.find_last_not_of(" \t") + 1); // Trim right

		// Construct the full texture path
		std::string fullPath = directory + (texturePath[0] == '\\' ? "" : "\\") + texturePath;
		std::replace(fullPath.begin(), fullPath.end(), '/', '\\'); // Normalize to backslashes

		TextureNew texture;
		texture.id = loadTextureFromFile(fullPath.c_str(), directory);
		texture.type = typeName;
		texture.path = fullPath;

		texturesLoaded.push_back(texture);

		logger.AddLog("Loaded texture: " + fullPath, ImGuiLogger::LogType::Assimp);
	}

	return texturesLoaded;
}


bool modelLoader::createShaderProgram() {

	logger.AddLog("Creating Buffers for SubMesh FBO");
	glGenFramebuffers(1, &submeshFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, submeshFBO);

	// Create a texture to attach to the FBO
	glGenTextures(1, &subMeshTexture);
	glBindTexture(GL_TEXTURE_2D, subMeshTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Attach the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, subMeshTexture, 0);


	GLuint depthRBO;
	glGenRenderbuffers(1, &depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);  // Use 24-bit depth and 8-bit stencil
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRBO);


	// Check if FBO is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind FBO

	logger.AddLog("Attempting to create shader program if it doesn't exist");
	if (shaderProgram == nullptr) {
		const std::string vertexShaderSource = R"(

30 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

		// Fragment Shader Source Code (inline)
		const std::string fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.1, 0.1, 0.7); // Default color (red)
}
)";

		shaderProgram = new Shader("modelvertex.glsl", "modelfragment.glsl");
		
		//GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
		//GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

		//// Link shaders to create a program
		//shaderProgram->ID = glCreateProgram();
		//glAttachShader(shaderProgram->ID, vertexShader);
		//glAttachShader(shaderProgram->ID, fragmentShader);
		//glLinkProgram(shaderProgram->ID);

		//// Check for linking errors
		//if (!checkLinkErrors(shaderProgram->ID)) {
		//	std::cerr << "Shader program linking failed!" << std::endl;
		//	return false;
		//}
		//else {
		//	std::cout << "No linking errors" << std::endl;
		//}

		//// Shaders are now part of the program, we can delete them
		//glDeleteShader(vertexShader);
		//glDeleteShader(fragmentShader);
		//

	}
	return true;
}

GLuint modelLoader::compileShader(GLenum shaderType, const std::string& source) {
	GLuint shader = glCreateShader(shaderType);
	const char* shaderSource = source.c_str();
	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);

	// Check for compile errors
	std::string shaderTypeStr = (shaderType == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
	if (!checkCompileErrors(shader, shaderTypeStr)) {
		glDeleteShader(shader);  // Don't need a broken shader
		return 0;
	}

	return shader;
}

bool modelLoader::checkCompileErrors(GLuint shader, const std::string& type) {
	GLint success;
	GLchar infoLog[1024];
	std::cout << "Checking Compile Errors" << std::endl;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
		std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
		return false;
	}
	return true;
	
}

bool modelLoader::checkLinkErrors(GLuint program) {
	GLint success;
	GLchar infoLog[1024];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 1024, nullptr, infoLog);
		std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
		return false;
	}
	return true;
}

void modelLoader::useShader() {
	glUseProgram(shaderProgram->ID);
}


unsigned int modelLoader::CreatePlaceholderTexture() {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Create a white 1x1 texture
	unsigned char whitePixel[4] = { 122, 255, 255, 255 }; // RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
	return textureID;


}


// Function to show the material editor with ImGui
void modelLoader::showMaterialEditor(std::vector<std::unique_ptr<modelNew>>& modelNewVector, int& activeModelIndex, int& activeSubMeshIndex) {
	// Safety check: make sure the vector isn't empty
	if (modelNewVector.empty()) return;

	// Create a window in ImGui
	ImGui::Begin("Material Editor");
	ImGui::SliderFloat("Delta Time Global", &deltaTimeGlobal, 0.0, 3000.0);
	// Step 1: Create a dropdown to select the active model
	std::vector<std::string> modelNames;
	for (int i = 0; i < modelNewVector.size(); ++i) {
		modelNames.push_back(modelNewVector[i]->name.empty() ? "Unnamed Model " + std::to_string(i) : modelNewVector[i]->name);
	}

	// ImGui dropdown for selecting the active model
	if (ImGui::BeginCombo("Select Model", modelNames[activeModelIndex].c_str())) {
		for (int i = 0; i < modelNames.size(); ++i) {
			bool isSelected = (activeModelIndex == i);
			if (ImGui::Selectable(modelNames[i].c_str(), isSelected)) {
				activeModelIndex = i;
				activeSubMeshIndex = 0; // Reset the submesh index when changing models
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	modelNew& activeModel = *modelNewVector[activeModelIndex];
	static modelNew* activeModelPtr = modelNewVector[activeModelIndex].get();
	// Step 2: Create a dropdown for the submeshes of the active model
	std::vector<std::string> subMeshNames;
	for (int i = 0; i < activeModel.subMeshes.size(); ++i) {
		subMeshNames.push_back(activeModel.subMeshes[i]->name.empty() ? "Unnamed SubMesh " + std::to_string(i) : activeModel.subMeshes[i]->name);
	}

	// ImGui dropdown for selecting the active submesh
	if (ImGui::BeginCombo("Select SubMesh", subMeshNames[activeSubMeshIndex].c_str())) {
		for (int i = 0; i < subMeshNames.size(); ++i) {
			bool isSelected = (activeSubMeshIndex == i);
			if (ImGui::Selectable(subMeshNames[i].c_str(), isSelected)) {
				activeSubMeshIndex = i;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Checkbox("Render Selected Submesh Separately", &boolRenderSubmesh);
	ImGui::Checkbox("Dislay Extra Animation Data", &boolDisplayExtraAnimationData);

	// Get the active submesh and its material
// Assuming activeModel is a pointer to modelNew
MeshNew* activeSubMesh = activeModel.subMeshes[activeSubMeshIndex].get(); // Access the raw pointer
	Material& material = activeSubMesh->material;
	
	submeshRendered = activeSubMesh;
	VAOtoRender = activeSubMesh->VAO; //WHEN WE HAVE LIGHTS IN THE SCENE, THE WRONG VAO IS RENDERED FOR A SUBMESH
	logger.AddLog(std::to_string(VAOtoRender), ImGuiLogger::LogType::Assimp);
	//THIS WILL BE THE LOCATION OF THE ERROR
	subMatrixView = activeModel.worldTransform;
	sumMeshVAOSize = activeModel.subMeshes.size();
	// Now let's draw the world transform window near the selected submesh

 // Step 4: Calculate the screen space position of the submesh using view and projection matrices
	glm::vec4 worldPosition = activeModel.worldTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // Get world position of the submesh
	glm::vec4 clipSpacePos = projection * camera.getViewMatrix() * worldPosition; // Convert to clip space
	glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w; // Normalize the coordinates

	// Convert NDC to screen space coordinates (assuming ImGui uses a window size resolution)
	ImVec2 screenPos = ImVec2(
		ImGui::GetIO().DisplaySize.x /2,
		ImGui::GetIO().DisplaySize.y /2
	);

	// Step 5: Create an ImGui window at the calculated screen position
	ImGui::SetNextWindowPos(screenPos);
	//ImGui::SetNextWindowSize(ImVec2(200, 150)); // Customize the size of the window

	// Step 6: Draw a window for world transform
	ImGui::Begin("World Transform", NULL, ImGuiWindowFlags_NoTitleBar |  ImGuiWindowFlags_NoBackground);

	// Step 7: Display 3 sliders for XYZ translation of the world transform
	glm::vec3 translation = activeModel.GetPosition(); // Access the translation part of the transform matrix
	glm::vec3 originalTranslation = translation;
	ImGui::Text("Move Model:");
	if (ImGui::SliderFloat("Move X", &translation.x, -200.0f, 200.0f) ||
		ImGui::SliderFloat("Move Y", &translation.y, -200.0f, 200.0f) ||
		ImGui::SliderFloat("Move Z", &translation.z, -200.0f, 200.0f)) {

		// Set the modified translation back to the model's world transform matrix
		activeModel.SetPosition(translation);
	}
	ImGui::Text("Rotate Model");
	float currentXRotation = activeModel.GetXRotation(); // Get current X rotation
	float currentYRotation = activeModel.GetYRotation(); // Get current Y rotation
	float currentZRotation = activeModel.GetZRotation(); // Get current Z rotation
	if (ImGui::SliderFloat("Rotate X", &currentXRotation, -2.0f, 2.0f)) {
		activeModel.SetXRotation(currentXRotation); // Set new X rotation
	}

	if (ImGui::SliderFloat("Rotate Y", &currentYRotation, -2.0f, 2.0f)) {
		activeModel.SetYRotation(currentYRotation); // Set new Y rotation
	}

	if (ImGui::SliderFloat("Rotate Z", &currentZRotation, -2.0f, 2.0f)) {
		activeModel.SetZRotation(currentZRotation); // Set new Z rotation
	}
	
	// Create variables to hold the Euler angles

	ImGui::End(); // End the world transform window
	// Step 3: Use ImGui sliders and color pickers to display and modify the material properties
	ImGui::Text("Material Properties");

	// Ambient color
	ImGui::ColorEdit3("Ambient Color", glm::value_ptr(material.ambient));

	// Diffuse color
	ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(material.diffuseColor));

	// Specular color
	ImGui::ColorEdit3("Specular Color", glm::value_ptr(material.specularColor));

	// Emissive color
	ImGui::ColorEdit3("Emissive Color", glm::value_ptr(material.emissiveColor));

	// Shininess
	ImGui::SliderFloat("Shininess", &material.shininess, 0.0f, 256.0f);

	// Transparency
	ImGui::SliderFloat("Transparency", &material.transparency, 0.0f, 1.0f);

	// Index of Refraction
	ImGui::SliderFloat("Index of Refraction", &material.indexOfRefraction, 1.0f, 3.0f);

	// Illumination model (assuming it's an integer)
	ImGui::InputInt("Illumination Model", &material.illuminationModel);

	ImGui::Separator();
	ImGui::Text("Animation Information");

	// Assuming `animations` is a vector containing all animations for the model
	if (animationsNew.empty()) {
		ImGui::Text("No animations available.");
	}
	else {
		ImGui::Text("Total Animations: %d", (int)animationsNew.size());
		for (int i = 0; i < animationsNew.size(); ++i) {
			const AnimationData& animData = animationsNew[i];
			ImGui::Text("Animation %d: Duration: %.2f, Ticks Per Second: %.2f", i + 1, animData.duration, animData.ticksPerSecond);
			ImGui::Text("Bone Animations: %d", (int)animData.boneAnimations.size());
			for (int j = 0; j < animData.boneAnimations.size(); ++j) {
				ImGui::Text("  Bone %d: %s", j + 1, animData.boneAnimations[j].boneName.c_str());
				ImGui::Text("  Position Keys: %d, Rotation Keys: %d, Scaling Keys: %d",
					(int)animData.boneAnimations[j].positions.size(),
					(int)animData.boneAnimations[j].rotations.size(),
					(int)animData.boneAnimations[j].scales.size());
			}
		}
	}

	ImGui::End(); // End the ImGui window

}


bool AnimationController::isValidForAnimation(const aiNode* rootNode) {
	// Perform checks specific to the animation controller to ensure validity
	if (rootNode == nullptr || boneTransforms.empty()) {
		return false;
	}
	// Additional logic can be added here
	return true;
}