#pragma once

#include "frameBufferObject.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "waterRenderer.h"


struct UIVertex {
    glm::vec2 position;
    glm::vec2 texCoord;
};


struct UITexture {
    GLuint textureID;
    RawModel rawModel;  // You may need additional information about the quad (size, position, etc.)

    UITexture(GLuint textureID, const RawModel& rawModel)
        : textureID(textureID), rawModel(rawModel) {}
};


class UIManager {
private:
    std::vector<UITexture> uiTextures;
    FrameBufferObject fbo;
    GLuint UIshaderProgram;

public:
    UIManager(int screenWidth, int screenHeight, bool useDepthBuffer = true);
  //  ~UIManager();

   void addTexture(std::string& name, const int sizeX, const int sizeY, const int posX, const int posY);
    void addTextureFromFBO(GLuint fboTextureID, int width, int hight, float x, float y, float quadWidth, float quadHeight);
    void renderUI();
    RawModel createQuadModel(float x, float y, float width, float height);
};


