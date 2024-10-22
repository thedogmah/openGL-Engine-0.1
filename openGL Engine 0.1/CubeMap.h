#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include "Shader.h"
#include "globals.h"
class CubeMap {
public:
    // Constructor and Destructor
    unsigned int skyboxVAO, skyboxVBO;
    std::vector<std::string> faces;
    std::vector<float> skyboxVertices = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    CubeMap();
    ~CubeMap();

    // Function to load the cube map textures
    void loadCubeMap();

    // Bind and Unbind functions
    void bind(unsigned int unit = 0) const;
    void unbind() const;

    // Fun utility functions
    void flipTexture(unsigned int face);
    void rotateTexture(unsigned int face, float angle);


    void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    // Getters
    unsigned int getID() const;

private:
    unsigned int textureID; // OpenGL texture ID
    unsigned int loadTexture(const std::string& path, unsigned int face);
    Shader cubemapShader;
    // Utility to handle texture manipulation (e.g., flip, rotate)
    void manipulateTexture(unsigned int face, const std::string& operation, float angle = 0.0f);
};

#endif

