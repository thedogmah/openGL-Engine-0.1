#pragma once


#include <vector>
#include <iostream>
#include "globals.h"
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

class Texture {
    public:
        Texture(const std::string& filePath, GLenum textureType);
      
        bool Load();
        void Bind(GLenum textureUnit) const;
        void Unbind() const;
        float sizeX, sizeY, posX, posY;

    private:
        GLuint textureID; // OpenGL texture ID
        GLenum type;     // Texture type (e.g., GL_TEXTURE_2D)
        std::string filePath;
        
  };


