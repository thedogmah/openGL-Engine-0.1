#pragma once

#include "Loader.h"        // Include the Loader header
#include "waterShader.h"  
#include "Camera.h"        // Include the Camera header
#include "WaterTile.h"     // Include the WaterTile header

void checkGLError(const char* functionName);

class RawModel {
public:
    RawModel(GLuint vaoID, GLsizei vertexCount, float x, float y, float width, float height)
        : vaoID(vaoID), vertexCount(vertexCount), x(x), y(y), width(width), height(height) {}

    GLuint getVaoID() const { return vaoID; }
    GLsizei getVertexCount() const { return vertexCount; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

private:
    GLuint vaoID;
    GLsizei vertexCount;
    float x, y;       // Position
    float width, height;  // Size
};
class WaterRenderer {
public:
    WaterRenderer(Loader& loader, waterShader& shader, const glm::mat4& projectionMatrix);
    void render(const std::vector<WaterTile>& water, const Camera& camera);
    void unbind();
    void bindShader();
    GLuint getShaderID();
private:
    RawModel* quad;
    waterShader shader;

    void prepareRender(const Camera& camera);

    void setUpVAO(Loader& loader);

   
};

