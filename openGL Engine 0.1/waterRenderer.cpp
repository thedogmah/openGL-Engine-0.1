#include "waterShader.h"
#include "waterRenderer.h"
#include "Loader.h"
#include "stb_image_write.h"
#include <stb/stb_image.h>

WaterRenderer::WaterRenderer(Loader& loader, waterShader& shader, const glm::mat4& projectionMatrix)
{
    shader.use();
    shader.loadProjectionMatrix(projectionMatrix);
    
    setUpVAO(loader);
   // shader.unbind();
}

void WaterRenderer::render(const std::vector<WaterTile>& water, const Camera& camera) {
    prepareRender(camera);
    
    glUniform1i(glGetUniformLocation(shader.ID, "fboTexture"), 14);  // Assuming texture unit 0

    // Set up generic projection and view matrices
    //glm::mat4  = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)1400 / (float)800, 0.1f, 30000.0f);

    glm::mat4 viewMatrix = camera.getViewMatrix();

        for (const auto& tile : water) {
            // Center the model at the origin, scale it, and position it
            glm::vec3 tilePosition(tile.getX(), tile.getHeight(), tile.getZ());

          

            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(tile.getX(), tile.getHeight(), tile.getZ()))
                * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f))
                * glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 5.0f));

        // Pass the matrices to the shader as uniforms
        shader.loadProjectionMatrix(projectionMatrix);
        shader.loadViewMatrix(camera);
        shader.loadModelMatrix(modelMatrix);

        glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount());
        checkGLError("glDrawArrays");
    }

   // unbind();
}

void WaterRenderer::prepareRender(const Camera& camera) {
    glUseProgram(shader.ID);
    std::cout << "Shader ID:" << shader.ID;
    shader.bindAttributes();

    shader.loadViewMatrix(camera);
    glBindVertexArray(quad->getVaoID());
    glEnableVertexAttribArray(0);
    checkGLError("prepareRender");
}

void WaterRenderer::unbind() {
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    shader.unbind();
    checkGLError("unbind");
}

void WaterRenderer::setUpVAO(Loader& loader) {
    // Just x and z vertex positions here, y is set to 0 in the vertex shader
    std::vector<GLfloat> vertices = { -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, 1 };
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