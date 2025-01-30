#include "waterShader.h"
#include "waterRenderer.h"
#include "Loader.h"
#include "stb_image_write.h"
#include <stb/stb_image.h>
#include <chrono>
#include "globals.h"
WaterRenderer::WaterRenderer(Loader& loader, waterShader& shader, const glm::mat4& projectionMatrix)
{
    shader.use();
    shader.loadProjectionMatrix(projectionMatrix);
  
    setUpVAO(loader);
    dudvTexture = loader.loadTexture(DUDVMap);
   // shader.unbind();
}

void WaterRenderer::render(const std::vector<WaterTile>& water, const Camera& camera) {
   // shader.use();
    if (!boolShowWater)//dont render water
        return;
    auto currentTime = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime.time_since_epoch());

    globalTime = duration.count();

    //std::cout << "\ncam position in water render before "<< camera.mPosition.y;
    prepareRender(camera);
   // std::cout << "\ncam position in water render 2 " << camera.mPosition.y;
    shader.connectTextureUnits();
    
    glActiveTexture(GL_TEXTURE14);

    // Bind your reflection texture to the active texture unit
    glBindTexture(GL_TEXTURE_2D, dudvTexture);

    // Set the uniform value for the reflection texture
//    glUniform1i(glGetUniformLocation(shader.ID, "fboTextureReflection"), 15);
    glProgramUniform1i(shader.ID, glGetUniformLocation(shader.ID, "DUDVmap"), 14);
 
    // Activate texture unit 14 for reflection texture
    glActiveTexture(GL_TEXTURE15);

    // Bind your reflection texture to the active texture unit
    glBindTexture(GL_TEXTURE_2D, reflectionTextureID);

    // Set the uniform value for the reflection texture
//    glUniform1i(glGetUniformLocation(shader.ID, "fboTextureReflection"), 15);
    glProgramUniform1i(shader.ID, glGetUniformLocation(shader.ID, "fboTextureReflection"), 15);
    // Activate texture unit 15 for refraction texture
    glActiveTexture(GL_TEXTURE16);

    // Bind your refraction texture to the active texture unit
    glBindTexture(GL_TEXTURE_2D, refractionTextureID);

    // Set the uniform value for the refraction texture
    glProgramUniform1i(shader.ID, glGetUniformLocation(shader.ID, "fboTextureRefraction"), 16);
   // glProgramUniform1i(glGetUniformLocation(shader.ID, "fboTextureRefraction"), 16);
    //glUniform1i(glGetUniformLocation(shader.ID, "fboTexture"), 14);  // Assuming texture unit 0
   
    // Set up generic projection and view matrices
    //glm::mat4  = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 30000.0f);
   // n = glm::perspective(glm::radians(45.0f) (float)window_width / (float)window_height, 0.1f, 30000.0f);
    //glm::mat4 viewMatrix = camera.getViewMatrix();
    ;
        for (const auto& tile : water) {
            // Center the model at the origin, scale it, and position it
            glm::vec3 tilePosition(tile.getX(), tile.getHeight(), tile.getZ());

          

            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(tile.getX(), tile.getHeight(), tile.getZ()))
                * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f))
                * glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 5.0f));

        // Pass the matrices to the shader as uniforms
        shader.loadProjectionMatrix(projectionMatrix);
    //    shader.setMatrixUniform(debugger.shaderProgram, camera);
        shader.loadModelMatrix(modelMatrix);
        //std::cout << "\ncam position in water render model matrix " << camera.mPosition.y;
        glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount());
        checkGLError("glDrawArrays");
    }

   // unbind();
}

void WaterRenderer::prepareRender(const Camera& camera) {
    glUseProgram(shader.ID);
    std::cout << "Shader ID:" << shader.ID;
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << std::endl;
    }
    shader.bindAttributes();
    
    shader.loadViewMatrix(camera);
    glBindVertexArray(quad->getVaoID());
    glEnableVertexAttribArray(0);


}

void WaterRenderer::unbind() {
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    shader.unbind();
    checkGLError("unbind");
}

void WaterRenderer::setUpVAO(Loader& loader) {
    // Just x and z vertex positions here, y is set to 0 in the vertex shader
    std::vector<GLfloat> vertices = { -1, -1, 1, -1, -1, 1,  // First triangle (clockwise)
                                  1, -1, -1, 1, 1, 1 };  // Second triangle (clockwise)

//std::vector<GLfloat> vertices = { -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, 1 };
    quad = new RawModel(loader.loadToVao(vertices, 2), vertices.size() / 2,0,0,0,0);
    checkGLError("setUpVAO");
    shader.bindAttributes();
}

void WaterRenderer::bindShader()
{
    glUseProgram(shader.ID);
}

GLuint WaterRenderer::getShaderID() {
    return this->shader.ID;
}