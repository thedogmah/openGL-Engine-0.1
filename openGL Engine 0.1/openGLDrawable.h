#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "globals.h"

struct Vertex;
class openGLDrawable
{
public:   

   
    
  
    openGLDrawable(std::vector<Vertex>& vertices);
  
 

    void draw();
    void update();
    
   // ~openGLDrawable();


private:
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices;

};

