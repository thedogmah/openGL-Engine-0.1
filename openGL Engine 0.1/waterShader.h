#pragma once



#include "Shader.h"
#include "Camera.h"
void checkGLError(const char* functionName);
class waterShader : public Shader {
public:
    waterShader();
    const char* VERTEX_FILE = "waterVertex.txt";
    const char* FRAGMENT_FILE = "waterFragment.txt";

    //Shader shader;
    void loadProjectionMatrix(const glm::mat4& projection);
    void loadViewMatrix(const Camera& camera);
    void loadModelMatrix(const glm::mat4& modelMatrix);
    void connectTextureUnits();

    GLint location_DUDVmap;
    GLint location_modelMatrix;
    GLint location_viewMatrix;
    GLint location_projectionMatrix;
    GLint location_fboTextureReflection;    
    GLint location_fboTextureRefraction;
    GLint locationTime;
    virtual void bindAttributes() override;
    virtual void getAllUniformLocations() override;
};

//#endif // WATER_SHADER_HPP