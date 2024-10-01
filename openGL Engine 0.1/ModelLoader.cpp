#include "ModelLoader.h"
#include "stb_image_write.h"
#include <stb/stb_image.h>
struct TextureNew;
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
	if (!scene) {
		logger.AddLog("Error: Scene is null.", ImGuiLogger::LogType::Assimp);
		return;
	}

	unsigned int numVerts = 0, numIndices = 0;

	// First, count all vertices and indices from all meshes
	countVertAndIndices(scene, numVerts, numIndices);

	// Then, reserve space for vertices and indices
	reserveSpace(numVerts, numIndices);

	// Finally, process the root node to initialize all meshes
	processNode(scene->mRootNode, scene);

	logger.AddLog("Initialized All Meshes", ImGuiLogger::LogType::Assimp);
}

void modelLoader::initSingleMesh(const aiMesh* meshPtr)
{

	
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
			}
			else {
				// Default texture coordinates if not available
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			// Store the vertex in the vector
			meshVertices.push_back(vertex);
		}

		// Process indices
		for (unsigned int i = 0; i < meshPtr->mNumFaces; ++i) {
			const aiFace& face = meshPtr->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				meshIndices.push_back(face.mIndices[j]);
			}
		}

		// Insert the processed vertices and indices into the class member vectors
		vertices.insert(vertices.end(), meshVertices.begin(), meshVertices.end());
		indices.insert(indices.end(), meshIndices.begin(), meshIndices.end());

		// Log the successful initialization of the mesh
		logger.AddLog("Initialized single mesh with " + std::to_string(meshVertices.size()) + " vertices and " + std::to_string(meshIndices.size()) + " indices", ImGuiLogger::LogType::Assimp);
	}





bool modelLoader::initMaterials(const aiScene* scenePtr, const std::string& filename)
{

	std::string directory = filename.substr(0, filename.find_last_of('\\'));
	
	std::replace(directory.begin(), directory.end(), '/', '\\');
	for (int i = 0; i < scenePtr->mNumMaterials; ++i) {
		aiMaterial* material = scenePtr->mMaterials[i];

		// Load diffuse textures
		std::vector<TextureNew> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// Log the number of diffuse textures loaded
		logger.AddLog("Loaded " + std::to_string(diffuseMaps.size()) + " diffuse textures for material " + std::to_string(i), ImGuiLogger::LogType::Assimp);

		// Load specular textures
		std::vector<TextureNew> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// Log the number of specular textures loaded
		logger.AddLog("Loaded " + std::to_string(specularMaps.size()) + " specular textures for material " + std::to_string(i), ImGuiLogger::LogType::Assimp);
		
		// Load normal textures
		std::vector<TextureNew> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", directory);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// Log the number of normal textures loaded
		logger.AddLog("Loaded " + std::to_string(normalMaps.size()) + " normal textures for material " + std::to_string(i), ImGuiLogger::LogType::Assimp);


	}

	logger.AddLog("Initialized materials from file: " + filename, ImGuiLogger::LogType::Assimp);
	// Log the number of materials
	logger.AddLog("Total number of materials: " + std::to_string(scenePtr->mNumMaterials), ImGuiLogger::LogType::Assimp);

	
	return true;
}

void modelLoader::Render(unsigned int shaderProgram)
{
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
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


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
	const std::string vertexShaderSource = R"(
#version 330 core
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

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	// Link shaders to create a program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for linking errors
	if (!checkLinkErrors(shaderProgram)) {
		std::cerr << "Shader program linking failed!" << std::endl;
		return false;
	}
	else {
		std::cout << "No linking errors" << std::endl;
	}

	// Shaders are now part of the program, we can delete them
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
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
	glUseProgram(shaderProgram);
}