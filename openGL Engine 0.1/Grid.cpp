#include <GLAD/glad.h>
#include "Shader.h"
#include "imgui.h"  // Assuming ImGui is properly included in your project
#include "Grid.h"
#include "globals.h"
#include "glm/glm.hpp"
    // Constructor: initialize the shader using the shader sources
WorldGrid::WorldGrid() : Shader("worldGrid.vert", "worldGrid.frag"), gridshader("worldGrid.vert", "worldGrid.frag") {
    for (int i = -gridSize; i <= gridSize; i++) {
        vertices.push_back(glm::vec3(i, 0, -gridSize));
        vertices.push_back(glm::vec3(i, 0, gridSize));
    }
    // Grid lines along Z-axis
    for (int i = -gridSize; i <= gridSize; i++) {
        vertices.push_back(glm::vec3(-gridSize, 0, i));
        vertices.push_back(glm::vec3(gridSize, 0, i));
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(glm::vec3), &vertices[0],
        GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

  
}

    // Function to draw the grid
    void WorldGrid::draw(glm::mat4 proj) {
        glUseProgram(gridshader.ID);

       // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 30000.0f);
        glm::mat4 viewMatrix = camera.getViewMatrix();
        glm::mat4 gVP = proj * viewMatrix;

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 30000.0f);

        gridshader.setMatrixUniform(gridshader.ID, viewMatrix);
        gridshader.setUniform("projection", projection);
        
        gridshader.setUniform("gCameraWorldPos", camera.getPosition());
        gridshader.setUniform("gGridSize", gridSize);
        gridshader.setUniform("gGridMinPixelsBetweenCells", gridMinPixelsBetweenCells);
        gridshader.setUniform ("gGridCellSize", gridCellSize);
        gridshader.setUniform("gGridColorThin", gridColorThin);
        gridshader.setUniform("gGridColorThick", gridColorThick);


        glBindVertexArray(VAO);

        // Draw the grid lines
        glDrawArrays(GL_LINES, 0, vertices.size());

        glBindVertexArray(0);
        

       
      //  glUseProgram(0);
    }

    // Function to create ImGui controls for the grid
    void WorldGrid::drawGridUI() {

        
        if(ImGui::CollapsingHeader("Grid Options"))
        {

        // Option to adjust grid color (thick lines)
        ImGui::ColorEdit4("Grid Color Thin", (float*)&gridColorThin);
        ImGui::ColorEdit4("Grid Color Thick", (float*)&gridColorThick);

        // Option to adjust grid size
        ImGui::SliderFloat("Grid Size", &gridSize, 10.0f, 200.0f); // Adjust min/max as needed

        // Option to adjust minimum pixels between grid cells
        ImGui::SliderFloat("Min Pixels Between Cells", &gridMinPixelsBetweenCells, 0.01f, 10.0f); // Adjust min/max as needed

        // Option to adjust grid cell size
        ImGui::SliderFloat("Grid Cell Size", &gridCellSize, 0.1f, 2); // Adjust min/max as needed

        // Option to adjust grid spacing
        ImGui::SliderFloat("Grid Spacing", &gridSpacing, 0.1f, 10.0f); // Adjust min/max as needed

        // Option to adjust grid line thickness
        ImGui::SliderFloat("Grid Thickness", &gridThickness, 0.11f, 50.1f); // Adjust min/max as needed
        ImGui::Checkbox("Show Grid", &boolShowGrid);
        }
           
        }

    

