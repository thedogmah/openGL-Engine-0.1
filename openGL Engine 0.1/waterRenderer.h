#ifndef WATER_RENDERER_HPP
#define WATER_RENDERER_HPP

#include "Loader.h"        // Include the Loader header
#include "waterShader.h"  
#include "Camera.h"        // Include the Camera header
#include "WaterTile.h"     // Include the WaterTile header
void checkGLError(const char* functionName);

class RawModel {
public:
    RawModel(GLuint vaoID, GLsizei vertexCount)
        : vaoID(vaoID), vertexCount(vertexCount) {}

    GLuint getVaoID() const { return vaoID; }
    GLsizei getVertexCount() const { return vertexCount; }

private:
    GLuint vaoID;
    GLsizei vertexCount;
};
class WaterRenderer {
public:
    WaterRenderer(Loader& loader, waterShader& shader, const glm::mat4& projectionMatrix);
    void render(const std::vector<WaterTile>& water, const Camera& camera);
    void unbind();
private:
    RawModel* quad;
    waterShader shader;

    void prepareRender(const Camera& camera);

    void setUpVAO(Loader& loader);

   
};

#endif // WATER_RENDERER_HPP
