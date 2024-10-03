#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Texture.h" // Assuming you have a texture loader class
#include "globals.h"
#include "ImGuiLogger.h"
class ObjImporter


{
public:
    //Geometry data
    ObjImporter();//constructors
    ~ObjImporter();

    bool loadOBJ(const std::string& filename);
    // Loads an MTL (material) file
    bool loadMTL(const std::string& filename);
    // Sets up the vertex buffers for rendering
    void setupBuffers();
    // Draws the loaded object using the specified shader
    void draw(unsigned int shader);
    //Function for parsing the obj file
    void parseOBJLine(const std::string& line);
// load texture of OBJ file
    void loadTexture(const std::string& filename);

    GLuint VAO, VBO[3], EBO;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    GLuint shaderProgram;
    GLuint textureID;

    

    bool createShaderProgram();

    GLuint compileShader(GLenum shaderType, const std::string& source);
    bool checkCompileErrors(GLuint shader, const std::string& type);
    bool checkLinkErrors(GLuint program);
    void useShader();
};

