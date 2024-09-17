#include "UIManager.h"
#include "waterRenderer.h"

UIManager::UIManager(int screenWidth, int screenHeight, bool useDepthBuffer)
    : fbo(screenWidth, screenHeight, useDepthBuffer), UIshaderProgram(0) {

   Shader UIShader("UIvertex.glsl", "UIfrag.glsl");
   UIshaderProgram = UIShader.ID;
   std::cout << "\nUI constructor shader ID:" << UIshaderProgram;
   glUseProgram(UIshaderProgram);

   GLint uiTextureLocation = glGetUniformLocation(UIshaderProgram, "uiTexture");

}


void UIManager::addTexture(std::string& name, const int sizeX, const int sizeY, const int posX, const int posY) {
//    //Texture newTexture(name, GL_TEXTURE_2D); //create a texture place holder within the ui class instead
//    ////of using the texture class, since requirements are differnet, are use an override.
//    //newTexture.sizeX = sizeX;
//    //newTexture.sizeY = sizeY;
//    //newTexture.posX = posX;
//    //newTexture.posY = posY;
//
//    //uiTextures.push_back(newTexture);
}

void UIManager::addTextureFromFBO(GLuint fboTextureID, int width, int hight, float x, float y, float quadWidth, float quadHeight)
{
    RawModel quadModel = createQuadModel(x, y, quadWidth, quadHeight);
    UITexture entry(fboTextureID, quadModel);
    uiTextures.push_back(entry);
    std::cout <<"\n" << entry.rawModel.getVaoID() << "< VAOID of quad and teture ID: " << entry.textureID;
    std::cout <<  "fbo ID passed to UI addtexture function: " << fboTextureID;
    std::cout << "UI Manager Shader ID: " << this->UIshaderProgram;
}

void UIManager::renderUI()
{
    glUseProgram(UIshaderProgram);
    glViewport(200, 150, window_width, window_height);
   //glBindTexture(GL_TEXTURE_2D, 0);
    

    // Iterate through UI textures and render each quad
    // Iterate through UI textures and render each quad
    //for (int unit = 0; unit < 15; ++unit) {
    //    glActiveTexture(GL_TEXTURE0 + unit);
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //    // Optionally clear other texture types (e.g., GL_TEXTURE_CUBE_MAP)
    //    // glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    //}int initialTextureUnit = 14;

    glBindFramebuffer(GL_FRAMEBUFFER,0);
   // glBindFrameBuffer(2, window_width, window_height);

    // Activate texture unit 14 and bind the reflection texture
    glActiveTexture(GL_TEXTURE0 + 13);
    glBindTexture(GL_TEXTURE_2D, uiTextures[0].textureID);
    //std::cout << "\n" << uiTextures[0].textureID << " << bound texture ID <<";
    // Set the "uiTexture" uniform in the shader to texture unit 14
    glUniform1i(glGetUniformLocation(UIshaderProgram, "uiTexture"), 13);
    glProgramUniform1i(UIshaderProgram, glGetUniformLocation(UIshaderProgram, "uiTexture"), 13);

    // Bind the VAO for the reflection UI element
    glBindVertexArray(uiTextures[0].rawModel.getVaoID());

    // Draw the UI element for reflection
    glDrawArrays(GL_TRIANGLES, 0, uiTextures[0].rawModel.getVertexCount());

 
    // Render to Refraction FBO (using text3re from vector position [1])
  //  glBindFramebuffer(GL_FRAMEBUFFER, 3);

    // Activate texture unit 15 and bind the refraction texture
    glActiveTexture(GL_TEXTURE0 + 13);
    glBindTexture(GL_TEXTURE_2D, uiTextures[1].textureID);
    //std::cout << "\n" << uiTextures[1].textureID << " << bound texture ID <<";
    // Set the "uiTexture" uniform in the shader to texture unit 15
    glUniform1i(glGetUniformLocation(UIshaderProgram, "uiTexture"), 13);

    // Bind the VAO for the refraction UI element
    glBindVertexArray(uiTextures[1].rawModel.getVaoID());

    // Draw the UI element for refraction
    
    glDrawArrays(GL_TRIANGLES, 0, uiTextures[1].rawModel.getVertexCount());

    // Unbind Refraction FBO
 //   unbindCurrentFrameBuffer();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


RawModel UIManager::createQuadModel(float x, float y, float width, float height) {
        GLuint vaoID, vboID;

        // Define vertices for a quad
        std::vector<UIVertex> quadVertices = {
    {glm::vec2(x, y), glm::vec2(0.0f, 1.0f)},
    {glm::vec2(x, y - height), glm::vec2(0.0f, 0.0f)},
    {glm::vec2(x + width, y - height), glm::vec2(1.0f, 0.0f)},

    {glm::vec2(x, y), glm::vec2(0.0f, 1.0f)},
    {glm::vec2(x + width, y - height), glm::vec2(1.0f, 0.0f)},
    {glm::vec2(x + width, y), glm::vec2(1.0f, 1.0f)}
};
        // Generate VAO and VBO
        glGenVertexArrays(1, &vaoID);
        glGenBuffers(1, &vboID);

        glBindVertexArray(vaoID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);

        // Store vertex data in the VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * quadVertices.size(), quadVertices.data(), GL_STATIC_DRAW);

        // Set up vertex attributes
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, texCoord));
        glEnableVertexAttribArray(1);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return RawModel(vaoID, quadVertices.size(), x, y, width, height);
};