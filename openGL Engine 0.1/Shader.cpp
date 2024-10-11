#pragma once
#include "Shader.h"
#include "ImGuiLogger.h"
class ImGuiLogger;

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // Log successful loading of shader files
        logger.AddLog("Successfully loaded vertex shader: " + std::string(vertexPath), ImGuiLogger::LogType::Shader);
        logger.AddLog("Successfully loaded fragment shader: " + std::string(fragmentPath), ImGuiLogger::LogType::Shader);
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        logger.AddLog("Failed to read shader files: " + std::string(vertexPath) + " or " + std::string(fragmentPath), ImGuiLogger::LogType::Shader);
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        logger.AddLog("Vertex shader compilation failed: " + std::string(infoLog), ImGuiLogger::LogType::Shader);
    }
    else {
        logger.AddLog("Vertex shader compiled successfully.", ImGuiLogger::LogType::Shader);
    }

    // Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        logger.AddLog("Fragment shader compilation failed: " + std::string(infoLog), ImGuiLogger::LogType::Shader);
    }
    else {
        logger.AddLog("Fragment shader compiled successfully.", ImGuiLogger::LogType::Shader);
    }

    // Shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertex); // Attach each shader individually
    glAttachShader(ID, fragment);
    glLinkProgram(ID); // Link the whole program when complete so it can be executed by CPU

    // Print any link errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        logger.AddLog("Shader program linking failed: " + std::string(infoLog), ImGuiLogger::LogType::Shader);
    }
    else {
        logger.AddLog("Shader program linked successfully.", ImGuiLogger::LogType::Shader);
    }

    // Delete the shaders since they are linked to the program and not necessary.
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    std::cout << "Paths: " << fragmentPath << ", " << vertexPath;
    std::cout << "\nShader constructor, program ID is:" << ID;
}

void Shader::use() {
    // Activate the ID assigned;
    glUseProgram(ID);
    std::cout << "Shader ID:" << ID;
    logger.AddLog("Using shader ID: " + std::to_string(ID), ImGuiLogger::LogType::Shader);
}


void Shader::setMatrixUniform(const GLint& shaderid, const glm::mat4& matrix) const {
    GLuint location = glGetUniformLocation(shaderid, "view");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));

    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "GL error: " << error << std::endl;
        logger.AddLog("OpenGL error encountered: " + std::to_string(error), ImGuiLogger::LogType::Shader);
    }
}

void Shader::unbind() {
    glUseProgram(0);
    logger.AddLog("Shader program unbound.", ImGuiLogger::LogType::Shader);
}
void Shader::bindAttributes() {
	// Implementation of bindAttributes
	// Example: glBindAttribLocation(ID, 0, "position");
}

void Shader::getAllUniformLocations() {
	// Implementation of getAllUniformLocations
	// Example: location_modelMatrix = glGetUniformLocation(ID, "modelMatrix");
}