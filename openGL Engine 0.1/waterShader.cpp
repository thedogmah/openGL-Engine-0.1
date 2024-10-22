#include "waterShader.h"
#include "stb_image_write.h"
#include <stb/stb_image.h>
#include "Camera.h"
waterShader::waterShader() : Shader("waterVertex.txt", "waterFragment.txt"){

    getAllUniformLocations();
    std::cout << "Water shader created\n";
    checkGLError("waterShader constructor");
    std::cout << "\n Location of water shader view matrix: " << location_viewMatrix;
 
}

void waterShader::bindAttributes() {
    glBindAttribLocation(ID, 0, "position");
    checkGLError("bindAttributes");
}

void waterShader::getAllUniformLocations() {
    std::cout << "\nGet all uniform locations function, water shader.";
    location_projectionMatrix = glGetUniformLocation(this->ID, "projection");
    location_viewMatrix = glGetUniformLocation(this->ID, "view");
    location_modelMatrix = glGetUniformLocation(this->ID, "model");
     location_fboTextureReflection = glGetUniformLocation(this->ID, "fboTextureReflection");
     location_fboTextureRefraction = glGetUniformLocation(this->ID, "fboTextureRefraction");
     location_DUDVmap = glGetUniformLocation(this->ID, "DUDVmap");
     locationTime = glGetUniformLocation(this->ID, "moveTime");
     glProgramUniformMatrix4fv(this->ID, location_projectionMatrix, 1, GL_FALSE, glm::value_ptr(projection));
     glProgramUniformMatrix4fv(this->ID, location_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
     glProgramUniformMatrix4fv(this->ID, location_modelMatrix, 1, GL_FALSE, glm::value_ptr(model));
     glProgramUniform1i(this->ID, location_fboTextureReflection, 15);
     glProgramUniform1i(this->ID, location_fboTextureRefraction, 16);
     glProgramUniform1i(this->ID, location_DUDVmap,14);
     
    checkGLError("getAllUniformLocations");
}


void waterShader::loadModelMatrix(const glm::mat4& modelMatrix) {
  //  this->getAllUniformLocations();
    glUniformMatrix4fv(location_modelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    checkGLError("loadModelMatrix");
}

void waterShader::loadProjectionMatrix(const glm::mat4& projection) {
   // this->getAllUniformLocations();
    glProgramUniformMatrix4fv(ID, location_projectionMatrix, 1, GL_FALSE, glm::value_ptr(projection));
    checkGLError("loadProjectionMatrix");
}

void waterShader::loadViewMatrix(const Camera& cam) {
    //this->getAllUniformLocations();
    location_viewMatrix = glGetUniformLocation(ID, "view");
    checkGLError("loadViewMatrix, get location view uniform");
    glm::mat4 view = cam.mViewMatrix;
   // glUseProgram(this->ID);
    glUniformMatrix4fv(location_viewMatrix, 1, GL_FALSE, glm::value_ptr(view));
    checkGLError("loadViewMatrix");
    std::cout << "\nLocation of water shader view matrix: " << location_viewMatrix;
}

void waterShader::connectTextureUnits()
{
   // float timeValue = glfwGetTime(); // Get time for animation
   // glUniform1f(glGetUniformLocation(shaderProgramID, "time"), timeValue);
    // Get the location of the uniform
    glUseProgram(ID);
    GLint sunColorLocation = glGetUniformLocation(ID, "sunColor");
        GLint timeLocation = glGetUniformLocation(ID, "Time");
        if (timeLocation != -1) {
            // Get time value for animation
            float timeValue = glfwGetTime(); // or whatever your global time is
            // Send the time value to the shader
            glUniform1f(timeLocation, timeValue);
            //std::cout << "Water shader found time uniform for rotating skycube/box";
        }
        else {
            // Handle error: uniform not found
         //   std::cout << "Uniform 'moveTime' not found in shader program!" << std::endl;
        }
    glUniform3fv(sunColorLocation, 1, &sun.DiffuseColor.x);

    //glUniform3fv(ID, glGetUniformLocation(ID, "sunColor"), &sun.DiffuseColor.x);
    glProgramUniform1i(ID, glGetUniformLocation(ID, "fboTextureReflection"), 15);
    glProgramUniform1i(ID, glGetUniformLocation(ID, "fboTextureRefraction"), 16);
    glProgramUniform1i(ID, glGetUniformLocation(ID, "DUDVmap"), 14);
    glProgramUniform1f(ID, glGetUniformLocation(ID, "moveTime"),globalTime);
}
