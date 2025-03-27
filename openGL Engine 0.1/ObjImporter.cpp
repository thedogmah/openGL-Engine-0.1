#include "ObjImporter.h"
#include <glm/glm.hpp>
#include "ImGuiLogger.h"
// Include the stb_image header

#include "stb/stb_image.h"


#include "stb_image_write.h"


ObjImporter::ObjImporter() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO);
    glGenBuffers(1, &EBO);
    logger.AddLog("ObjImporter Constructdor Called", ImGuiLogger::LogType::Info);

    }
bool ObjImporter::loadOBJ(const std::string& filename) {
 
    std::ifstream objFile(filename);
    if (!objFile.is_open()) {

        std::cout << "Didn't open file for Obj file - Obj Importer Class\n" << std::endl;
        return false;
    }

    std::string line;

    while (std::getline(objFile, line)) {
        parseOBJLine(line);
    }

    objFile.close();


    std::string mtlFilename;// = filename.substr(0, filename.find_last_of(".")) + ".mtl";
    std::ifstream mtlFile(mtlFilename);

    if (mtlFile.is_open()) {
        loadMTL(mtlFilename);
    }
    else {
        std::cerr << "MTL file not found: " << mtlFilename << std::endl;
    }

    mtlFile.close();

    std::cout << "Loaded OBJ file: " << filename << std::endl;
   // std::cout << "Ve  rtices: " << vertices.size() << ", Normals: " << normals.size() << ", Indices: " << indices.size() << std::endl;

      // After loading the OBJ file, create the shader program
    if (!createShaderProgram()) {
        std::cerr << "Failed to create shader program after loading OBJ file." << std::endl;
        return false;
    }

    
    setupBuffers(); 
    std::cout << "Buffers setup";
    return true;
}
 

bool ObjImporter::loadMTL(const std::string& filename)
{
    std::ifstream mtlFile(filename);
    if (!mtlFile.is_open()) {
        std::cerr << "Could not open MTL file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(mtlFile, line)) {
        std::istringstream sstream(line);
        std::string identifier;
        sstream >> identifier;

        if (identifier == "map_Kd") { // Diffuse texture map
            std::string textureFile;
            sstream >> textureFile;
            loadTexture(textureFile);
        }
    }

    mtlFile.close();
    return true;
}



void ObjImporter::setupBuffers() {

    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    // Normals buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
    std::cout << "Vertices size: " << vertices.size() << std::endl;
    std::cout << "Texture coordinates size: " << texCoords.size() << std::endl;
    std::cout << "Normals size: " << normals.size() << std::endl;
    std::cout << "Indices size: " << indices.size() << std::endl;

}



    void ObjImporter::draw(unsigned int shader) {
      //  useShader(); // Use your custom shader program
        glUseProgram(shader);
        // --- Set Model, View, Projection Matrices ---
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix, object stays at origin
        glm::mat4 view = camera.getViewMatrix(); // Get the view matrix from the camera
        //glm::mat4 projection =  glm::perspective(glm::radians (45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
        //glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 30000.0f);  //glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

        // Upload the matrices to the shader
        GLint modelLoc = glGetUniformLocation(shader, "model");
        GLint viewLoc = glGetUniformLocation(shader, "view");
        GLint projLoc = glGetUniformLocation(shader, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Ensure there are indices to draw
        if (indices.empty()) {
            std::cerr << "No indices available for drawing." << std::endl;
            return;
        }
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
        // --- Render the object ---
        glBindVertexArray(VAO);
       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind the Element Buffer Object

       glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Draw using vertices without EBO
 
        glBindVertexArray(0);
    }

    


    

void ObjImporter::parseOBJLine(const std::string& line)
{
   
        std::istringstream
            sstream(line);
        std::string identifier;
        sstream >> identifier;
        std::ostringstream logMessage;
        logMessage << "Face: ";

        if (identifier == "v") {
            glm::vec3 vertex;
            sstream >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (identifier == "vn") {
            glm::vec3 normal;
            sstream >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (identifier == "vt") {
            glm::vec2 texCoord;
            sstream >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        }
        int counter = 0; // Counter to track which index we're handling
        unsigned int temp_glint; // Temporary variable to hold the index

        while (sstream >> temp_glint) {
            // Push the position index (adjust for zero-based indexing)
            indices.push_back(temp_glint);
            logMessage << temp_glint << " ";

            // Check the next character for slashes
            if (sstream.peek() == '/') {
                sstream.ignore(); // Ignore the slash
                counter++; // Move to the next index type

                // If there's another slash, we have a normal index
                if (sstream.peek() == '/') {
                    sstream.ignore(); // Ignore the second slash
                    counter++; // Move to the next index type
                }
            }
            else {
                counter++; // No slashes means we just encountered a position index
            }

            // Limit to 3 indices for a triangular face
            if (counter >= 3) {
                break;
            }
        }

        logger.AddLog(logMessage.str());
    
}

void ObjImporter::loadTexture(const std::string& filename)
{
    int width, height, nrChannels;

    // Load the image data using stb_image
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        // Generate and bind the texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Upload the texture to the GPU
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set the texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Free the image data after uploading it to the GPU
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        stbi_image_free(data);
    }
}


ObjImporter::~ObjImporter() {
   /* glDeleteBuffers(3, VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);*/
    logger.AddLog("Obj Importer Destructor Called");
}

bool ObjImporter::createShaderProgram() {
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
    FragColor = vec4(1.0, 0.1, 0.1, 0.7); // Default color (white)
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
    else { std::cout << "No linking errors"; }

    // Shaders are now part of the program, we can delete them
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

GLuint ObjImporter::compileShader(GLenum shaderType, const std::string& source) {
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

bool ObjImporter::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    std::cout << "Checking Compile Errors";
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        return false;
    }
    return true;
}

bool ObjImporter::checkLinkErrors(GLuint program) {
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

void ObjImporter::useShader() {
    glUseProgram(shaderProgram);

}