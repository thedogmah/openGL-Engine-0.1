#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>    // Assimp Importer
#include <assimp/scene.h>         // Assimp Scene
#include <assimp/postprocess.h>   // Post-processing flags
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>         // GLM math library
//#include <GLFW/glfw3.h>   

#include "globals.h"
#include "Shader.h"
struct VertexNew;
struct TextureNew;
struct MeshNew;
static int meshNewId = 0;

struct modelNew {
public:
    std::vector<MeshNew> subMeshes; // Holds submeshes for this model
    std::string name;                // Optional: Name of the model
    int modelID;                     // Unique identifier for the model

    modelNew() {} // Ensure model ID is unique
};
struct MeshNew {
public:
    std::vector<VertexNew> vertices;       // Vertex data
    std::vector<unsigned int> indices;     // Index data
    std::vector<TextureNew> textures;      // Texture data

    GLuint VAO;                            // Vertex Array Object
    GLuint VBO;                            // Vertex Buffer Object
    GLuint EBO;                            // Element Buffer Object

    int meshID;
    std::string name;                      // Name for each mesh.

    // Default constructor
    MeshNew() {};

    // Copy constructor
    MeshNew(const MeshNew& other)
        : vertices(other.vertices),         // Copy vertex data
        indices(other.indices),           // Copy index data
        textures(other.textures),         // Copy texture data
        VAO(other.VAO),                   // Copy Vertex Array Object
        VBO(other.VBO),                   // Copy Vertex Buffer Object
        EBO(other.EBO),                   // Copy Element Buffer Object
        meshID(other.meshID),             // Copy mesh ID
        name(other.name)                   // Copy name
    {
        // The copy constructor will automatically use the default copy
        // semantics for STL containers (std::vector, std::string).
    }
};

struct VertexNew {
    glm::vec3 Position;    // Vertex position
    glm::vec3 Normal;      // Vertex normal
    glm::vec2 TexCoords;   // Texture coordinates
};

struct TextureNew {
    unsigned int id;       // Texture ID
    std::string type;      // Texture type (diffuse, specular, etc.)
    std::string path;      // Path to the texture file
};

class modelLoader {
public:
    modelLoader() {

        worldTransform = glm::mat4(1.0f);
    }
//~modelLoader();
    

    void clear();//Function to clear the model data.
    
    //init model from assimp scene
    bool initFromScene(const aiScene* scenePtr, const std::string& filename);

    void countVertAndIndices(const aiScene* scenePtr, unsigned int& numVerts, unsigned int& numIndices);

    void reserveSpace(unsigned int numVertices, unsigned int numIndices);// reserve space for vectors

    // Initialize all meshes in the scene
    void initAllMeshes(const aiScene* scene);

    // Initialize a single mesh
    void initSingleMesh(const aiMesh* meshPtr);

    // Initialize materials for the model
    bool initMaterials(const aiScene* scenePtr, const std::string& filename);

    // Setters for model transformations
    void SetRotation(const glm::vec3& rotationAngles);
    void SetScale(const glm::vec3& scale);
    void SetTranslation(const glm::vec3& translation);
    void setupBuffersNew(MeshNew& meshData);
    // Render the model using the specified shader program
    void Render(unsigned int shaderProgram);

    bool createShaderProgram();
    void useShader();

    static Shader* shaderProgram;
    
   

    GLuint compileShader(GLenum shaderType, const std::string& source);
    bool checkCompileErrors(GLuint shader, const std::string& type);
    bool checkLinkErrors(GLuint program);
    
    std::vector<MeshNew> meshes; //vector to store all mesh struct data
    std::vector<modelNew*> modelNewVector;

    glm::mat4 worldTransform;                    // Transformation matrix for the model
    std::vector<VertexNew> vertices;                // Vertex data
    std::vector<unsigned int> indices;           // Index data
    std::vector<TextureNew> textures;               // Texture data
    
    // Helper functions
    unsigned int loadTextureFromFile(const char* path, const std::string& directory);
    void processNode(aiNode* node, const aiScene* scene);
    std::vector<TextureNew> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory);
};