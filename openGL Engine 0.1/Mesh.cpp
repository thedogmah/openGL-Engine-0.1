#include "Mesh.h"
std::vector<std::string> Mesh::meshNames;
Mesh::~Mesh()

{
	// Clean up resources associated with textures
	for (auto texture : textures) {
		delete texture;
	}
	textures.clear();

	// Clear other vectors
	positions.clear();
	normals.clear();
	texcoords.clear();
	indices.clear();
	meshes.clear();

	// Delete OpenGL buffers
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (EBO != 0) {
		glDeleteBuffers(1, &EBO);
		EBO = 0;
	}
}


void Mesh::Render(unsigned int shader)
{
	std::cout << "\n" << meshName;
	glUseProgram(this->customShaderProgramID);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//this->shaderProgram = shader;
	/*GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error before setting matrices: " << error << std::endl;
	}*/
	// Query the uniform location for modelUniform
	GLuint modelMatrixLocation = glGetUniformLocation(this->customShaderProgramID, "modelUniform");

	// Create a translation matrix based on the provided translation vector
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

	// Set the worldTransform to the identity matrix
	worldTransform = glm::mat4(1.0f);

	// Apply the transformations in the correct order: translation, rotation, scale
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	// Update the world transform matrix with the scale, rotation, and translation
	worldTransform = translationMatrix * rotationMatrix * scaleMatrix;



	// Update the model matrix uniform in your shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(worldTransform));
	//std::cout << "\nShader program for render functions is: " << shader << std::endl;

	glm::mat4 viewMatrix; // Your view matrix

	// Get the uniform location for the view matrix in the shader
	GLuint viewMatrixLocation = glGetUniformLocation(shader, "view");

	// Pass the view matrix to the shader
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(cameraPtr->getViewMatrix()));


	for (int i = 0; i < 13; i++) {
		glActiveTexture(GL_TEXTURE0 + i); // Set the active texture unit
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
	}//	error = glGetError();
	/*if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error before binding shaderprogram: " << error << std::endl;
	}*/

	
	/*error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "Error directly after binding shader program in mesh.cpp render " << error << std::endl;
	}*/

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, 0);
	/*error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error before mesh loop: " << error << std::endl;
	}*/

	glActiveTexture(GL_TEXTURE0);
	//std::cout << "\nMesh debug output. Mesh vector size: " << meshes.size();
	for (unsigned int i = 0; i < meshes.size(); i++) {
		unsigned int materialIndex = meshes[i].materialIndex;
		//std::cout << "\nLoop iteration: " << i;
	/*	error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error before assert: " << error << std::endl;
		}*/
		assert(materialIndex < textures.size());
		/*GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error before bind texture unit: " << error << std::endl;
		}*/
		if (textures[materialIndex]) {//Checks if there is a material without a diffuse texture.
			textures[materialIndex]->Bind(COLOUR_TEXTURE_UNIT);
			//std::cout << "\nBound texture unit: " << i;
		}
		/*error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error after binding texture unit: " << error << std::endl;
		}*/
		//std::cout << "\nChecked texture.";
		//GLDrawElBaseVertex allows us to draw sub regions. We provide an offset below (void * sizeof etc)
		glDrawElementsBaseVertex(GL_TRIANGLES, meshes[i].numIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * meshes[i].baseIndex),
			meshes[i].baseVertex);

		//glBindTexture(GL_TEXTURE_2D, 0);

	//	std::cout << "\nFirst draw call success " << i;
	}
	std::cout << "\n" << meshName << "^^.\n";
	glBindTexture(GL_TEXTURE_2D, 0);
	//std::cout << "\nFinished mesh draw call function successfully";
	glBindVertexArray(0);
	//if (scenePtr)
	//{
	//	std::cout << "sceneptr  valid";
	//}
	//else std::cout << "scene ptr not valid(at mesh render function)";
	}

	void Mesh::renderInstance(unsigned int shader, GLuint ssboID, GLuint amount)
	{
	
		//std::cout << "\nSSBO ID to render: " << ssboID << "Amount of times is " << amount << std::endl;
		//std::cout << "\n" << meshName << std::endl << "\n";
		glUseProgram(shader);
		GLint view = glGetUniformLocation(shader, "view");

		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//this->shaderProgram = shader;
		//GLenum error = glGetError();
		//if (error != GL_NO_ERROR) {
		//	std::cerr << "OpenGL error before setting matrices: " << error << std::endl;
		//}
		//// Query the uniform location for modelUniform
		GLuint modelMatrixLocation = glGetUniformLocation(shader, "modelUniform");

		// Create a translation matrix based on the provided translation vector
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

		// Set the worldTransform to the identity matrix
		worldTransform = glm::mat4(1.0f);

		// Apply the transformations in the correct order: translation, rotation, scale
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

		// Update the world transform matrix with the scale, rotation, and translation
		worldTransform = translationMatrix * rotationMatrix * scaleMatrix;



		// Update the model matrix uniform in your shader
	//	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(worldTransform));
		//std::cout << "\nShader program for render functions is: " << shader << std::endl;

		glm::mat4 viewMatrix; // Your view matrix

		// Get the uniform location for the view matrix in the shader
		GLuint viewMatrixLocation = glGetUniformLocation(shader, "view");

		// Pass the view matrix to the shader
	//	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(cameraPtr->getViewMatrix()));
		GLuint isInstancedBool = glGetUniformLocation(shader, "isInstanced");

		glUniform1i(isInstancedBool, 1);

		for (int i = 0; i < 13; i++) {
			glActiveTexture(GL_TEXTURE0 + i); // Set the active texture unit
			glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
		}	/*error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error before binding shaderprogram: " << error << std::endl;
		}*/


		//error = glGetError();
		//if (error != GL_NO_ERROR) {
		//	std::cerr << "Error directly after binding shader program in mesh.cpp render " << error << std::endl;
		//}

		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, 0);
	/*	error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error before mesh loop: " << error << std::endl;
		}*/

		glActiveTexture(GL_TEXTURE0);
		//std::cout << "\nMesh debug output. Mesh vector size: " << meshes.size();

			
		for (int h = 0; h < meshes.size(); h++) {
			//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);	

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboID);
			for (unsigned int i = 0; i < 1; i++) {
				
				unsigned int materialIndex = meshes[h].materialIndex;
				//std::cout << "\nLoop iteration: " << i;

				assert(materialIndex < textures.size());

				if (textures[materialIndex]) {//Checks if there is a material without a diffuse texture.
					textures[materialIndex]->Bind(COLOUR_TEXTURE_UNIT);
					//std::cout << "\nBound texture unit: " << i;
				}

				//std::cout << "\nChecked texture.";
				//GLDrawElBaseVertex allows us to draw sub regions. We provide an offset below (void * sizeof etc)
				glDrawElementsInstancedBaseVertex(GL_TRIANGLES, meshes[h].numIndices, GL_UNSIGNED_INT,
					(void*)(sizeof(unsigned int) * meshes[h].baseIndex), amount, meshes[h].baseVertex);


				//glBindTexture(GL_TEXTURE_2D, 0);
			/*	error = glGetError();
				if (error != GL_NO_ERROR) {
					std::cerr << "OpenGL error before mesh loop: " << error << std::endl;
				}*/

			//	std::cout << "\nFirst draw call success " << i;
			}
		/*	error = glGetError();
			if (error != GL_NO_ERROR) {
				std::cerr << "OpenGL error before mesh loop: " << error << std::endl;
			}*/

		}
	
		glBindTexture(GL_TEXTURE_2D, 0);
		//std::cout << "\nFinished mesh draw call function successfully";
		glBindVertexArray(0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
		//glUniform1i(isInstancedBool, 0);
	}

void Mesh::loadMesh(const std::string filename, GLuint shader)
{
	this->worldTransform = glm::mat4(1.0f);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	//Create the buffers for the vertices attributes
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(buffers), buffers);

	bool ret = false;

	

	scenePtr = importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAG);

	if (scenePtr) {//if there is no error we call initfromScene to continue initialisation. Tidy code
		ret = initFromScene(scenePtr, filename);
		
		//std::cout << "\nScene pointer valid (this means file at least exits and initFromScene function called";
	}
	else {
		std::cout << "Error parsing '%s': '%s'\n", filename.c_str(), importer.GetErrorString();
	}
	glBindVertexArray(0);
	this->shaderProgram = shader;
	this->customShaderProgramID= shader;
}

void Mesh::populateBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, buffers[POS_VB]);//Gen buffers is used in the constructor, and an array is passed. 
	//continued...The enum allows us to name the buffers and remember which data type is stored in which number allocation. It is classic OOP 
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXCOORD]);//Gen buffers is used in the constructor, and an array is passed. 
	//continued...The enum allows us to name the buffers and remember which data type is stored in which number allocation. It is classic OOP 
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords[0]) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEXTURE_LOCATION);
	glVertexAttribPointer(TEXTURE_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_VB]);//Gen buffers is used in the constructor, and an array is passed. 
	//continued...The enum allows us to name the buffers and remember which data type is stored in which number allocation. It is classic OOP 
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

}


void Mesh::reserveSpace(unsigned int numVertices, unsigned int numIndices) {
	positions.reserve(numVertices);//pre allocates but doesnt increat size
	normals.reserve(numVertices);
	texcoords.reserve(numVertices);
	indices.reserve(numIndices);
}
bool Mesh::initFromScene(const aiScene* scenePtr, const std::string filename) {

	meshes.resize(scenePtr->mNumMeshes);
	textures.resize(scenePtr->mNumTextures);

	unsigned int numVertices = 0;
	unsigned int numIndices = 0;
	countVertandIndices(scenePtr, numVertices, numIndices);

	reserveSpace(numVertices, numIndices);

	initAllMesh(scenePtr);

	if (!initMaterials(scenePtr, filename)) {
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << error << std::endl;
		}
		return false;
	}
	populateBuffers();
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << error << std::endl;
	}
	return true;//GLCheckError();
}

void Mesh::initAllMesh(const aiScene* scene)
{
	//std::cout << "\nStart of init all mesh function, before loop.";
	for (unsigned int i = 0; i < meshes.size();   i++) {
		const aiMesh* paiMesh = scene->mMeshes[i];
		initSingleMesh(paiMesh);
		//std::cout << "\nInit all mesh count: " << i;
	}

}


void Mesh::initSingleMesh(const aiMesh* meshPtr){
//	std::cout << "\nStarted init of single meshes.";
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < meshPtr->mNumVertices; i++) {
		aiVector3D pos = meshPtr->mVertices[i];
		aiVector3D normal = meshPtr->mNormals[i];
		aiVector3D texcoord = meshPtr->HasTextureCoords(0) ? meshPtr->mTextureCoords[0][i] : Zero3D;
	
		positions.push_back(glm::vec3(pos.x, pos.y, pos.z));
		normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
		texcoords.push_back(glm::vec2(texcoord.x, texcoord.y));
		//std::cout << "\nInit single mesh count: " << i;
	}


//populat the index buffer
	for (unsigned int i = 0; i < meshPtr->mNumFaces; i++) {
		const aiFace& face = meshPtr->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
}
bool Mesh::initMaterials(const aiScene* scenePtr, const std::string& filename)
{
	std::string::size_type slashIndex = filename.find_last_of("/");
	std::string dir;

	if (slashIndex == std::string::npos) {
		dir = ".";
	}
	else if (slashIndex == 0) {
		dir = "/";
	}
	else
		dir = filename.substr(0, slashIndex);
	bool ret = true;
	textures.resize(scenePtr->mNumMaterials);
	for (unsigned int i = 0; i < scenePtr->mNumMaterials; i++) {
		const aiMaterial* material = scenePtr->mMaterials[i];
		textures[i] = NULL;

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string p = path.data;
				if (p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}
				std::string fullPath = dir + "/" + p;
				if (boolPathOverride) {
					fullPath = stringMeshPathOverride.c_str() + p;
					std::cout << fullPath;
				}
				std::cout << fullPath; // Ensure both parts are correct

				textures[i] = new Texture(fullPath.c_str(), GL_TEXTURE_2D);
				std::cout << fullPath;
				if (!textures[i]->Load()) {
					std::cout << "texture not loaded";
				}
				else
					std::cout << "\nTexture loaded";
				
			}
		}
	}

	return ret;
}

void Mesh::clear()
{


}

void Mesh::countVertandIndices(const aiScene* scenePtr, unsigned int& numVerts, unsigned int& numIndices)
{

	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].materialIndex = scenePtr->mMeshes[i]->mMaterialIndex;
		meshes[i].numIndices = scenePtr->mMeshes[i]->mNumFaces * 3; //Has already been triangulated in AIscene setup
		meshes[i].baseVertex = numVerts;
		meshes[i].baseIndex = numIndices;

		numVerts += scenePtr->mMeshes[i]->mNumVertices;
		numIndices += meshes[i].numIndices;
}
}// Set the scale of the object
void Mesh::SetScale(const glm::vec3& scale) {
	// Query the uniform location for modelUniform
	glUseProgram(this->customShaderProgramID);
	GLuint modelMatrixLocation = glGetUniformLocation(this->customShaderProgramID, "modelUniform");

	// Create a scaling matrix based on the provided scale factors
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	// Set the worldTransform to the identity matrix
	worldTransform = glm::mat4(1.0f);

	// Apply the transformations in the correct order: translation, rotation, scale
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Update the world transform matrix with the scale, rotation, and translation
	worldTransform = translationMatrix * rotationMatrix * scaleMatrix;

	// Update the model matrix uniform in your shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(worldTransform));
}

// Set the rotation of the object
void Mesh::SetRotation(const glm::vec3& rotationAngles) {
	// Query the uniform location for modelUniform
	glUseProgram(this->customShaderProgramID);
	GLuint modelMatrixLocation = glGetUniformLocation(this->customShaderProgramID, "modelUniform");

	// Create a rotation matrix based on the provided angles
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Set the worldTransform to the identity matrix
	worldTransform = glm::mat4(1.0f);

	// Apply the transformations in the correct order: translation, rotation, scale
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	// Update the world transform matrix with the scale, rotation, and translation
	worldTransform = translationMatrix * rotationMatrix * scaleMatrix;

	// Update the model matrix uniform in your shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(worldTransform));
}

// Set the translation of the object
void Mesh::SetTranslation(const glm::vec3& translation) {
	// Query the uniform location for modelUniform
	glUseProgram(this->customShaderProgramID);
	GLuint modelMatrixLocation = glGetUniformLocation(this->customShaderProgramID, "modelUniform");

	// Create a translation matrix based on the provided translation vector
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

	// Set the worldTransform to the identity matrix
	worldTransform = glm::mat4(1.0f);

	// Apply the transformations in the correct order: translation, rotation, scale
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	// Update the world transform matrix with the scale, rotation, and translation
	worldTransform = translationMatrix * rotationMatrix * scaleMatrix;

	// Update the model matrix uniform in your shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(worldTransform));
}