#include "ModelLoader.h"
#include "stb_image_write.h"
#include <stb/stb_image.h>
struct TextureNew;

Shader* modelLoader::shaderProgram = nullptr;

void modelLoader::clear() {

	vertices.clear();
	indices.clear();
	textures.clear();

}

bool modelLoader::initFromScene(const aiScene* scenePtr, const std::string& filename) {

	if (!scenePtr || scenePtr->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scenePtr->mRootNode)
	{
			std::cout << "Error: Assimp could not load the model from: " << filename << std::endl;
		logger.AddLog("Assimple could not load the model", ImGuiLogger::LogType::Assimp);
		return false;
	}

	initAllMeshes(scenePtr);

	if (!initMaterials(scenePtr, filename)) {
		std::cout << "Error init materials from: " << filename << std::endl;
	}

	for (auto& mesh : meshes) {
		//setupBuffersNew(mesh);
		
	}
	
	createShaderProgram();
	
	
	
	return true;


}

void modelLoader::countVertAndIndices(const aiScene* scenePtr, unsigned int& numVerts, unsigned int& numIndices) {
	numVerts = 0;
	numIndices = 0;


	for (unsigned int i = 0; i < scenePtr->mNumMeshes; ++i) {

		const aiMesh* meshPtr = scenePtr->mMeshes[i];

		numVerts += meshPtr->mNumVertices;
		numIndices += meshPtr->mNumFaces * 3; //Each face in aiMesh is a triangle

	}
	std::stringstream logMessage;
	logMessage << "Counted Vertices: " << numVerts << ", Indices: " << numIndices;
	logger.AddLog(logMessage.str().c_str(), ImGuiLogger::LogType::Assimp);



}

void modelLoader::reserveSpace(unsigned int numVertices, unsigned int numIndices)
{
	vertices.reserve(numVertices);
	indices.reserve(numIndices);

	logger.AddLog("Reserved space for " + std::to_string(numVertices) + " vertices and " 
	+ std::to_string(numIndices) + " indices", ImGuiLogger::LogType::Assimp);

}
void modelLoader::initAllMeshes(const aiScene* scene)
{
	meshNewId++;
	modelNew* model;
	model = new modelNew();
	if (!scene) {
		logger.AddLog("Error: Scene is null.", ImGuiLogger::LogType::Assimp);
		return;
	}

	unsigned int numVerts = 0, numIndices = 0;

	// First, count all vertices and indices from all meshes
	countVertAndIndices(scene, numVerts, numIndices);

	// Then, reserve space for vertices and indices
	reserveSpace(numVerts, numIndices);
	modelNewVector.push_back(model);

	// Finally, process the root node to initialize all meshes
	processNode(scene->mRootNode, scene);

	logger.AddLog("Initialized All Meshes", ImGuiLogger::LogType::Assimp);
	modelNewVector.back()->modelID = meshNewId;
	modelNewVector.back()->name = scene->mRootNode->mName.C_Str();


}
void modelLoader::initSingleMesh(const aiMesh* meshPtr)
{
	MeshNew meshData; // create the whole instance to push back.
	meshData.name = meshPtr->mName.C_Str(); // Use the mesh name from Assimp
	meshData.meshID = meshNewId;
	std::vector<VertexNew> meshVertices;
	std::vector<unsigned int> meshIndices;

	// Check if meshPtr is valid
	if (!meshPtr) {
		logger.AddLog("Error: meshPtr is null.", ImGuiLogger::LogType::Assimp);
		return; // Handle error accordingly
	}

	// Process vertices in loop
	for (unsigned int i = 0; i < meshPtr->mNumVertices; ++i) {
		VertexNew vertex;
		// Ensure we are accessing vertex positions
		vertex.Position = glm::vec3(meshPtr->mVertices[i].x, meshPtr->mVertices[i].y, meshPtr->mVertices[i].z);

		// Check if normals are available
		if (meshPtr->HasNormals()) {
			vertex.Normal = glm::vec3(meshPtr->mNormals[i].x, meshPtr->mNormals[i].y, meshPtr->mNormals[i].z);
		}
		else {
			// Default normal value if not available
			vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		// Texture Coordinates (if there's a texture)
		if (meshPtr->mTextureCoords[0]) {
			vertex.TexCoords = glm::vec2(meshPtr->mTextureCoords[0][i].x, meshPtr->mTextureCoords[0][i].y);
			// Log the UV coordinates
			logger.AddLog("Vertex " + std::to_string(i) + " UVs: (" +
				std::to_string(vertex.TexCoords.x) + ", " +
				std::to_string(vertex.TexCoords.y) + ")", ImGuiLogger::LogType::Shader);
		}
		else {
			// Default texture coordinates if not available
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// Log the default UV coordinates
			logger.AddLog("Vertex " + std::to_string(i) + " UVs: (0.0, 0.0)", ImGuiLogger::LogType::Shader);
		}

		// Store the vertex in the vector
		meshData.vertices.push_back(vertex);
		
	}

	// Process indices
	for (unsigned int i = 0; i < meshPtr->mNumFaces; ++i) {
		const aiFace& face = meshPtr->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			meshData.indices.push_back(face.mIndices[j]);
		}
	}

	// Insert the processed vertices and indices into the class member vectors
	vertices.insert(vertices.end(), meshVertices.begin(), meshVertices.end());
	indices.insert(indices.end(), meshIndices.begin(), meshIndices.end());

	// Log the successful initialization of the mesh
	logger.AddLog("Initialized single mesh with " + std::to_string(meshVertices.size()) + " vertices and " + std::to_string(meshIndices.size()) + " indices", ImGuiLogger::LogType::Assimp);
	
	setupBuffersNew(meshData);
	modelNewVector.back()->subMeshes.push_back((meshData));

	meshes.push_back((meshData));

}





bool modelLoader::initMaterials(const aiScene* scenePtr, const std::string& filename)
{
	//retreive the latest created Mesh Data
	MeshNew& lastMesh = meshes.back(); // Reference to the last mesh added


	std::string directory = filename.substr(0, filename.find_last_of('\\'));
	
	std::replace(directory.begin(), directory.end(), '/', '\\');
	for (int i = 0; i < scenePtr->mNumMaterials; ++i) {
		aiMaterial* material = scenePtr->mMaterials[i];

		// Load diffuse textures
		std::vector<TextureNew> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		lastMesh.textures.insert(lastMesh.textures.end(), diffuseMaps.begin(), diffuseMaps.end()); // Add to the last mesh's textures

		// Log the number of diffuse textures loaded
		logger.AddLog("Loaded " + std::to_string(diffuseMaps.size()) + " diffuse textures for material " + std::to_string(i), ImGuiLogger::LogType::Assimp);

		// Load specular textures
		std::vector<TextureNew> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		lastMesh.textures.insert(lastMesh.textures.end(), specularMaps.begin(), specularMaps.end());

		// Log the number of specular textures loaded
		logger.AddLog("Loaded " + std::to_string(specularMaps.size()) + " specular textures for material " + std::to_string(i), ImGuiLogger::LogType::Assimp);
		
		// Load normal textures
		std::vector<TextureNew> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", directory);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		lastMesh.textures.insert(lastMesh.textures.end(), normalMaps.begin(), normalMaps.end());

		// Log the number of normal textures loaded
		logger.AddLog("Loaded " + std::to_string(normalMaps.size()) + " normal textures for material " + std::to_string(i), ImGuiLogger::LogType::Assimp);


	}

	logger.AddLog("Initialized materials from file: " + filename, ImGuiLogger::LogType::Assimp);
	// Log the number of materials
	logger.AddLog("Total number of materials: " + std::to_string(scenePtr->mNumMaterials), ImGuiLogger::LogType::Assimp);

	
	return true;
}

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
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (void*)offsetof(VertexNew, Position));
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (void*)offsetof(VertexNew, Normal));
	glEnableVertexAttribArray(1);

	// Texture coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (void*)offsetof(VertexNew, TexCoords));
	glEnableVertexAttribArray(2);

	// Unbind the VAO (it's good practice)
	glBindVertexArray(0);

	// Store the VAO in the meshData for later use in rendering
	mesh.VAO = VAO1;
	//.meshID
	
		

	
	
}

void modelLoader::Render(unsigned int shaderProgram) {
	// Use the shader program
	glUseProgram(this->shaderProgram->ID);
	GLint viewPosLoc = glGetUniformLocation(this->shaderProgram->ID, "viewPos"); // Get location for viewPos
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.getPosition()));
	// Set the view and projection uniforms
	GLint viewLoc = glGetUniformLocation(this->shaderProgram->ID, "view");
	GLint projLoc = glGetUniformLocation(this->shaderProgram->ID, "projection");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Loop through all the meshes (submeshes) and render them
	for (const MeshNew& mesh : meshes) {
		// Set the model matrix for each mesh
		glm::mat4 model = glm::mat4(1.0f);  // Identity matrix
		GLint modelLoc = glGetUniformLocation(this->shaderProgram->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Check if the current mesh has textures
		if (!mesh.textures.empty()) {
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1; // Specular texture counter

			for (unsigned int i = 0; i < mesh.textures.size(); i++) {
				glActiveTexture(GL_TEXTURE0 + i);  // Activate texture unit
				std::string number;
				std::string name = mesh.textures[i].type;

				if (name == "texture_diffuse") {
					number = std::to_string(diffuseNr++);  // Increment diffuse texture counter
				}
				else if (name == "texture_specular") {
					number = std::to_string(specularNr++);  // Increment specular texture counter
				}

				// Set the sampler in the shader
				glUniform1i(glGetUniformLocation(this->shaderProgram->ID, (name + number).c_str()), i);
				glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);  // Bind the texture
			}
		}
		else {
			// If no textures, unbind all possible bound textures to prevent issues
			for (unsigned int i = 0; i < 16; i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, 0);  // Unbind any textures
			}
			glActiveTexture(GL_TEXTURE0);  // Reset active texture unit to default
		}

		// Bind the VAO of the current mesh and draw it
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);  // Unbind VAO after drawing

		// Clean up texture bindings
		if (!mesh.textures.empty()) {
			for (unsigned int i = 0; i < mesh.textures.size(); i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture
			}
			glActiveTexture(GL_TEXTURE0);  // Reset active texture unit
		}
	}

	// Unbind the shader program
	glUseProgram(0);
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

			stbi_image_free(data);
		}
	return textureID;

}
void modelLoader::processNode(aiNode* node, const aiScene* scene)
{
	// Process each mesh in the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		initSingleMesh(mesh); // Initialize the mesh
	}

	// Recursively process each child node
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
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