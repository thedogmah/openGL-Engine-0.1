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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp> // For matrix transformations
#include <glm/gtc/quaternion.hpp>       // For quaternion operations
#include <glm/gtx/euler_angles.hpp>     // For Euler angle functions if needed
#include <glm/gtx/string_cast.hpp>    // For debugging (if you need to print glm types)
#include <random>
#include <memory>  // For std::unique_ptr
#include <stdexcept> // For std::runtime_error
#include "globals.h"
#include "Shader.h"
struct VertexNew;
struct TextureNew;
struct MeshNew;
static int meshNewId = 0;//a count of the given mesh ID as we add a unique mesh each time to vector

struct modelNew {
public:
    std::vector<std::unique_ptr<MeshNew>> subMeshes; // Holds pointers to sub-meshes for this model
    std::string name;                // Optional: Name of the model
    int modelID;                     // Unique identifier for the model
    //forward declare
    glm::mat4 worldTransform;        // World transformation matrix
    glm::mat4 GetModelMatrix() const {
        // Extract the position from the worldTransform matrix
        return worldTransform; // Return the position as a vec3
        std::vector<Material> materials;
    }
 
    void SetPosition(const glm::vec3& position) {
        // Set the X, Y, Z components of the translation (position) in the worldTransform matrix
        worldTransform[3] = glm::vec4(position, worldTransform[3].w); // Preserve the 'w' component
    }

    void SetRotation(const glm::mat3& rotation) {
        glm::mat4 rotation4x4 = glm::mat4(1.0f); // Initialize to identity
        rotation4x4[0] = glm::vec4(rotation[0], 0.0f); // Set first column (X-axis)
        rotation4x4[1] = glm::vec4(rotation[1], 0.0f); // Set second column (Y-axis)
        rotation4x4[2] = glm::vec4(rotation[2], 0.0f); // Set third column (Z-axis)

        // Assuming you want to keep the translation part of the worldTransform
        glm::vec3 translation = glm::vec3(worldTransform[3]); // Extract translation from worldTransform

        // Update the worldTransform with the new rotation while keeping the original translation
        worldTransform = rotation4x4; // Apply the rotation
        worldTransform[3] = glm::vec4(translation, 1.0f); // Reapply the translation
    }

    void SetEulerAngles(const glm::vec3& angles) {
        // Convert angles from degrees to radians
        float angleX = glm::radians(angles.x);
        float angleY = glm::radians(angles.y);
        float angleZ = glm::radians(angles.z);

        // Create rotation matrices for each axis
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1, 0, 0));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(0, 1, 0));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), angleZ, glm::vec3(0, 0, 1));

        // Combine rotations and update the world transform
        worldTransform = rotationZ * rotationY * rotationX * glm::mat4(worldTransform);
    }
    glm::vec3 GetEulerAngles() {
        glm::mat3 rotation = GetRotation(); // Get the rotation matrix

        float angleX = glm::atan(rotation[2][1], rotation[2][2]); // Rotation around X
        float angleY = glm::atan(-rotation[2][0], glm::sqrt(rotation[2][1] * rotation[2][1] + rotation[2][2] * rotation[2][2])); // Rotation around Y
        float angleZ = glm::atan(rotation[1][0], rotation[0][0]); // Rotation around Z

        return glm::vec3(glm::degrees(angleX), glm::degrees(angleY), glm::degrees(angleZ)); // Return angles in degrees
    }
    
    float GetXRotation() {
        glm::vec3 eulerAngles = GetEulerAngles();
        return eulerAngles.x;
    }

    float GetYRotation() {
        glm::vec3 eulerAngles = GetEulerAngles();
        return eulerAngles.y;
    }

    float GetZRotation() {
        glm::vec3 eulerAngles = GetEulerAngles();
        return eulerAngles.z;
    }
    void SetXRotation(float angle) {
        glm::vec3 angles = GetEulerAngles(); // Get current angles
        angles.x = angle; // Set new angle for X
        SetEulerAngles(angles); // Use existing method to set angles
    }

    void SetYRotation(float angle) {
        glm::vec3 angles = GetEulerAngles(); // Get current angles
        angles.y = angle; // Set new angle for Y
        SetEulerAngles(angles); // Use existing method to set angles
    }

    void SetZRotation(float angle) {
        glm::vec3 angles = GetEulerAngles(); // Get current angles
        angles.z = angle; // Set new angle for Z
        SetEulerAngles(angles); // Use existing method to set angles
    }

    //glm::vec3 GetEulerAngles() {
    //    glm::mat3 rotation = GetRotation(); // Get the rotation matrix

    //    float angleX = glm::atan(rotation[2][1], rotation[2][2]); // Rotation around X
    //    float angleY = glm::atan(-rotation[2][0], glm::sqrt(rotation[2][1] * rotation[2][1] + rotation[2][2] * rotation[2][2])); // Rotation around Y
    //    float angleZ = glm::atan(rotation[1][0], rotation[0][0]); // Rotation around Z

    //    return glm::vec3(glm::degrees(angleX), glm::degrees(angleY), glm::degrees(angleZ)); // Return angles in degrees
    //}
    glm::vec3& GetPosition() {
        // Extract the position from the worldTransform matrix
        return *reinterpret_cast<glm::vec3*>(&worldTransform[3]); // Cast vec4 to vec3 (modify directly)

    }

    glm::mat3& GetRotation() {
        // Set the X, Y, Z components of the translation (position) in the worldTransform matrix
        glm::mat3 rotationMatrix = glm::mat3(worldTransform);


        // Extract scaling factors (lengths of each column)
        //glm::vec3 scale;
        //scale.x = glm::length(rotationMatrix[0]); // Length of X-axis vector
        //scale.y = glm::length(rotationMatrix[1]); // Length of Y-axis vector
        //scale.z = glm::length(rotationMatrix[2]); // Length of Z-axis vector

        // Normalize the columns to get pure rotation (removes scaling)
        rotationMatrix[0] = glm::normalize(rotationMatrix[0]); // Normalize X axis
        rotationMatrix[1] = glm::normalize(rotationMatrix[1]); // Normalize Y axis
        rotationMatrix[2] = glm::normalize(rotationMatrix[2]); // Normalize Z axis


        // Return the rotation matrix and scale vector
        return  *reinterpret_cast<glm::mat3*>(&rotationMatrix);

    }
    modelNew()
        : worldTransform(glm::mat4(1.0f)) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> distrib(-20.0f,30.0f);

        // Generate random positions within the specified range
        float randomX = distrib(gen);
        float randomZ = distrib(gen);

        // Set the position in the worldTransform matrix
        worldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(randomX, 0.0f, randomZ));
    }// Initialize to identity matrix

    modelNew(const modelNew& other)
        : name(other.name), modelID(other.modelID), worldTransform(other.worldTransform) {
        // Perform a deep copy of subMeshes
        for (const auto& subMesh : other.subMeshes) {
            // Copy each MeshNew instance
            subMeshes.push_back(std::make_unique<MeshNew>(*subMesh));
        }
    }
};


struct MeshNew {
public:
    std::vector<VertexNew> vertices;       // Vertex data
    std::vector<unsigned int> indices;     // Index data
    std::vector<TextureNew> textures;      // Texture data
    Material material; // Store materials here

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
        name(other.name),   
        material(other.material)// Copy name
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


        //generate openGL resoureces for frame buffer for submesh inspector
       
    }
    //~modelLoader();


    void clear();//Function to clear the model data.

    //init model from assimp scene
    bool initFromScene(const aiScene* scenePtr, const std::string& filename);

    void countVertAndIndices(const aiScene* scenePtr, unsigned int& numVerts, unsigned int& numIndices);

    void reserveSpace(unsigned int numVertices, unsigned int numIndices);// reserve space for vectors

    // Initialize all meshes in the scene
    void initAllMeshes(const aiScene* scene, const std::string& filename);

    // Initialize a single mesh
    void initSingleMesh(const aiMesh* meshPtr, const aiScene* scene, const std::string& filename);
    int activeModelIndex = 0;//imgui data for clicking models to edit
    int activeSubMeshIndex = 0;//imgui data for clicking models to edit
    void renderSubmesh(MeshNew& model, GLuint VAO);
    // Initialize materials for the model
    //bool initMaterials(const aiScene* scenePtr, const std::string& filename);
    void showMaterialEditor(std::vector<std::unique_ptr<modelNew>>& modelNewVector, int& activemodelindex, int& activeSubmeshindex);
    Material processMaterial( aiMaterial* material, const aiScene* scene, const std::string& filename, std::vector<TextureNew>& textures);
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
    
    //scene shader details
    float ambientLightIntensity = 0.1f;
    glm::vec3 lightPosInput;

    //Submesh Inspector Resources
    GLuint submeshFBO, subMeshTexture;
    bool boolRenderSubmesh = false;
    MeshNew* submeshRendered;
    GLuint VAOtoRender;
    glm::mat4 subMatrixView;
    unsigned int sumMeshVAOSize = 0;//(How many submeshes)? this can be used by the FBO to loop through all parts
    //of the model every second or on a button press.
        GLuint compileShader(GLenum shaderType, const std::string& source);
    bool checkCompileErrors(GLuint shader, const std::string& type);
    bool checkLinkErrors(GLuint program);

    std::vector<MeshNew> meshes; //vector to store all mesh struct data
    std::vector<std::unique_ptr<modelNew>> modelNewVector; // Vector to hold models


    glm::mat4 worldTransform;                    // Transformation matrix for the model
    std::vector<VertexNew> vertices;                // Vertex data
    std::vector<unsigned int> indices;           // Index data
    std::vector<TextureNew> textures;               // Texture data

    // Helper functions
    unsigned int loadTextureFromFile(const char* path, const std::string& directory);
    void processNode(aiNode* node, const aiScene* scene, const std::string&  filename);
    std::vector<TextureNew> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory);
 
    unsigned int CreatePlaceholderTexture();
};


