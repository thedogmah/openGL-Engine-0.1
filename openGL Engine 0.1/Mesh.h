#pragma once
#define POSITION_LOCATION 0
#define TEXTURE_LOCATION 1
#define NORMAL_LOCATION 2
#define COLOUR_TEXTURE_UNIT 3

#include <vector>
#include <iostream>
#include "globals.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include "Texture.h"
#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class Texture;
class Mesh
{

public:
   // Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    Mesh() { this->worldTransform = glm::mat4(1.0f); };
    void Render(unsigned int shader);
    void loadMesh(const std::string filenane, GLuint shader);
    void Render(unsigned int numInstances, const glm::mat4 WVPmatrix, const glm::mat4 worldMatrix);
    void populateBuffers();
    glm::mat4 getWorldTransform() { return worldTransform; }
    void SetWorldTransform(const glm::mat4& newWorldTransform) {
        this->worldTransform = newWorldTransform;
    }
    const aiScene* scenePtr = nullptr;
public:


    enum BUFFER_TYPE {
        INDEX_BUFFER = 0,
        POS_VB = 1,
        TEXCOORD = 2,
        NORMAL_VB = 3,
        WVP_MAT_VB = 4,
        WORLD_MAT_VB = 5, //model matrix
        NUM_BUFFERS = 6
    
    };

    struct basicMeshEntry {
        basicMeshEntry() {
            numIndices = 0;
            baseVertex = 0;
            baseIndex = 0;
            materialIndex = 0;
        }
        unsigned int numIndices;
        unsigned int baseVertex;
        unsigned int baseIndex;
        unsigned int materialIndex;

    };
    Assimp::Importer importer;
    std::vector<basicMeshEntry> meshes;
    std::vector<Texture*> textures;
    GLuint buffers[NUM_BUFFERS] = { 0 };
    void clear();
    bool initFromScene(const aiScene* scenePtr, const std::string filename);

    void countVertandIndices(const aiScene* scenePtr, unsigned int& numVerts, unsigned int& numIndices);
    void reserveSpace(unsigned int numVertices, unsigned int numIndices);
    void initAllMesh(const aiScene* scene);
    void initSingleMesh(const aiMesh* meshPtr);
    bool initMaterials(const aiScene* scenePtr, const std::string& filename);
    
    //model engine setter functions
    void SetRotation(const glm::vec3& rotationAngles);

    // Set the scale of the object
    void SetScale(const glm::vec3& scale);

    // Set the translation of the object
    void SetTranslation(const glm::vec3& translation);

    
    // Vertex data
    // 
    //std::vector<Vertex> verticess;
    std::string meshName;//this instances name
    static std::vector<std::string> meshNames;//stores all mesh names for engine tools
    std::vector<unsigned int> indicess;
    std::vector<Texture> texturess; //from the original mesh class. not a pointer vector
    glm::mat4 worldTransform;
  
    glm::vec3 rotation = glm::vec3(0.0f);  // Initialize to zero rotation
    glm::vec3 translation = glm::vec3(0.0f);  // Initialize to zero translation
    glm::vec3 scale = glm::vec3(1.0f);  // Initialize to scale of 1.0 in all dimensions
    // OpenGL buffers and rendering-related data
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram; //currently bound shader program if any
    void setupMesh();

public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<unsigned int> indices;
};

