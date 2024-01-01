#include "waterShader.h"

waterShader::waterShader() : Shader("waterVertex.txt", "waterFragment.txt"), shader("waterVertex.txt", "waterFragment.txt") {

    getAllUniformLocations();
    std::cout << "Water shader created\n";
    checkGLError("waterShader constructor");
    
 
}

void waterShader::bindAttributes() {
    glBindAttribLocation(shader.ID, 0, "position");
    checkGLError("bindAttributes");
}

void waterShader::getAllUniformLocations() {
    location_projectionMatrix = glGetUniformLocation(shader.ID, "projectionMatrix");
    location_viewMatrix = glGetUniformLocation(shader.ID, "viewMatrix");
    location_modelMatrix = glGetUniformLocation(shader.ID, "modelMatrix");
    checkGLError("getAllUniformLocations");
}

void waterShader::loadProjectionMatrix(const glm::mat4& projection) {
    shader.setMatrixUniform(location_projectionMatrix, projection);
    checkGLError("loadProjectionMatrix");
}

void waterShader::loadViewMatrix(const Camera& cam) {
    glm::mat4 viewMatrix = cam.getViewMatrix();
    shader.setMatrixUniform(location_viewMatrix, viewMatrix);
    checkGLError("loadViewMatrix");
}

void waterShader::loadModelMatrix(const glm::mat4& modelMatrix) {
    shader.setMatrixUniform(location_modelMatrix, modelMatrix);
    checkGLError("loadModelMatrix");
}