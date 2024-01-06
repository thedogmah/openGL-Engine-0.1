#pragma once

#include "Loader.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "globals.h"
#include "stb_image_write.h"
#include <stb/stb_image.h>

GLuint Loader::loadToVao(const std::vector<GLfloat>& positions,
    const std::vector<GLfloat>& textureCoords,
    const std::vector<GLfloat>& normals,
    const std::vector<GLuint>& indices) {

    GLuint vaoID = 0;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    bindIndicesBuffer(indices);
    storeDataInAttributeList(0, 3, positions);
    storeDataInAttributeList(1, 2, textureCoords);
    storeDataInAttributeList(2, 3, normals);

    glBindVertexArray(0);
    return vaoID;
}

GLuint Loader::loadToVao(std::vector<GLfloat>& positions, int dimensions) {
    GLuint vaoID = 0;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    storeDataInAttributeList(0, dimensions, positions);

    glBindVertexArray(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        // Handle or log the error as needed
        std::cerr << "OpenGL error after loadToVao: " << error << std::endl;
    }

    return vaoID;
}

GLuint Loader::loadTexture(const std::string& textureFile) {
    int width, height, channels;
    
    
    
    
    stbi_set_flip_vertically_on_load(true); // Flip the image vertically
    unsigned char* image = stbi_load(("res/" + textureFile).c_str(), &width, &height, &channels, STBI_rgb_alpha);

    GLuint textureID = 0;
    if (image) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);
    }
    else {
        std::cerr << "Failed to load texture: " << textureFile << std::endl;
    }

    return textureID;
}

GLuint Loader::loadCubeMap(const std::vector<std::string>& textureFiles) {
    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    for (GLuint i = 0; i < textureFiles.size(); i++) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true); // Flip the image vertically
        unsigned char* image = stbi_load(("res/" + textureFiles[i]).c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (image) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            stbi_image_free(image);
        }
        else {
            std::cerr << "Failed to load texture: " << textureFiles[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texID;
}

void Loader::bindIndicesBuffer(const std::vector<GLuint>& indices) {
    GLuint vboID = 0;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void Loader::storeDataInAttributeList(int attributeNumber, int coordinateSize, const std::vector<GLfloat>& data) {
    GLuint vboID = 0;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attributeNumber, coordinateSize, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        // Handle or log the error as needed
        std::cerr << "OpenGL error: " << error << std::endl;
    }
}