#include "Cube.h"

Cube::Cube(GLuint shaderProgramID, float size, bool useTexture) : shaderProgramID(shaderProgramID), scale(1.0f)
{
    modelMatrix = glm::mat4(1.0f);
	//Generate Cube Vertices
	float halfSize = size / 2.0f;
    vertices = {
          -halfSize, -halfSize, -halfSize,  // 0
         halfSize, -halfSize, -halfSize,  // 1
         halfSize,  halfSize, -halfSize,  // 2
        -halfSize,  halfSize, -halfSize,  // 3
        -halfSize, -halfSize,  halfSize,  // 4
         halfSize, -halfSize,  halfSize,  // 5
         halfSize,  halfSize,  halfSize,  // 6
        -halfSize,  halfSize,  halfSize // 7

    };

    // Generate cube indices.
    indices = {
        0, 1, 2, 2, 3, 0, // Front face
        1, 5, 6, 6, 2, 1, // Right face
        7, 6, 5, 5, 4, 7, // Back face
        4, 0, 3, 3, 7, 4, // Left face
        4, 5, 1, 1, 0, 4, // Bottom face
        3, 2, 6, 6, 7, 3  // Top face
    };

    modelMatrixLoc = glGetUniformLocation(shaderProgramID, "model");
    setupMesh();

}

void Cube::draw()
{
   

   // glm::mat4 scaledModelMatrix = glm::scale(modelMatrix, glm::vec3(scale));

   // glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(scaledModelMatrix));



    //std::cout << "before draw vao bind";
   
   glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO

  //  std::cout << "after draw DRAW Elements";
    //glBindVertexArray(0);
    //// Unbind the VBO
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
  //  std::cout << "after glbind buffer in draw function";
}

void Cube::setScale(float scale)
{
    this->scale = scale;

}

void Cube::update()
{
    std::cout << "\nstart ofcube update cube.cpp";
    cubeProperties.rotation.y += 0.007;
    //order is reversed due to glm library multiplying matrices together before the vector.
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(cubeProperties.position));
   // modelMatrix = glm::rotate(modelMatrix, glm::vec3(cubeProperties.rotation).x, glm::vec3(1.f, 0.f, 0.f));
    modelMatrix = glm::rotate(modelMatrix, glm::vec3(cubeProperties.rotation).y, glm::vec3(0.f, 1.f, 0.f));
   // modelMatrix = glm::rotate(modelMatrix, glm::vec3(cubeProperties.rotation).z, glm::vec3(0.f, 0.f, 1.f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(cubeProperties.scale));
    std::cout << "\nEnd ofcube update cube.cpp";
   

     modelMatrixLoc = glGetUniformLocation(shaderProgramID, "model");
     glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void Cube::setCubeData(cubeData cubeData) 
{


    std::cout << "\nStart of setting up cube data in cube.cpp";
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(cubeData.position));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(cubeData.scale));
    modelMatrix = glm::rotate(modelMatrix, glm::vec3(cubeData.rotation).x,glm::vec3(1.f,0.f,0.f));
    modelMatrix = glm::rotate(modelMatrix, glm::vec3(cubeData.rotation).y, glm::vec3(0.f, 1.f, 0.f));
    modelMatrix = glm::rotate(modelMatrix, glm::vec3(cubeData.rotation).z, glm::vec3(0.f, 0.f, 1.f));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(cubeData.position));
    std::cout << "\nend of setting up cube data in cube.cpp";
}

void Cube::setTexture(GLuint textureID)
{
}

void Cube::setupMesh()
{
    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

    //glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); 
    //glEnableVertexAttribArray(0);


    ////Unbinding buffers
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //EBO
    //glBindVertexArray(0); //VAO
    //glBindBuffer(GL_ARRAY_BUFFER, 0); //VBO
 
}
