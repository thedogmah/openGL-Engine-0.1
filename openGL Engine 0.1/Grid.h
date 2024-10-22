#ifndef GRID_H
#define GRID_H

#include <glm/glm.hpp>
#include "Shader.h"
#include "imgui.h"  // Assuming ImGui is integrated into your project

class WorldGrid : public Shader {
public:

public:
    Shader gridshader;

    // Shader source code as class members
    unsigned int VAO, VBO, EBO;
    // Variables for customizing the grid via ImGui
    glm::vec4 gridColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);  // Default to light gray
    float gridSpacing = 2.0f;
    float gridThickness = 7.02f;
    // Constructor: Initializes the grid shader program
    WorldGrid();
    std::vector<glm::vec3>vertices;
    // Default values for grid-related variables
    glm::vec4 gridColorThin = glm::vec4(0.8f, 0.8f, 0.8f,0.7f);         // Light gray for thin grid lines
    glm::vec4 gridColorThick = glm::vec4(0.7f, 0.7f, 0.7f, 0.7f);        // Black for thick grid lines
    float gridSize = 100.0f;                                              // Size of the grid (can be adjusted based on the scene)
    float gridMinPixelsBetweenCells = 14.0f;                              // Minimum pixels between grid cells
    float gridCellSize = 0.1f;                                         // Size of each grid cell
    bool boolShowGrid = true;
    // Draws the grid, activates the shader, and calls glDrawArrays
    void draw(glm::mat4 proj);


    // Vertex and Fragment Shader source code (inline as const char* literals)

    // Grid customization options (modifiable via ImGui)
   

    // ImGui interface to customize grid options in real-time
    void drawGridUI();
};

#endif
