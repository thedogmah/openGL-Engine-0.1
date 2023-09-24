#pragma once

#define BT_USE_DOUBLE_PRECISION
#include "globals.h"
#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btVector3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"





    class PhysDebug : public btIDebugDraw
    {

    public:
        unsigned int shaderProgram;
        PhysDebug();
        void SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix);
        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
        void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);
        
        unsigned int* shaderPtr;

        // Implement the other pure virtual functions
        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {
            // Implementation...
        }

        void reportErrorWarning(const char* warningString) override {
            // Implementation...
        }

        void draw3dText(const btVector3& location, const char* textString) override {
            // Implementation...
        }

        void setDebugMode(int debugMode) override {
            m_debugMode = debugMode;
        }

        int getDebugMode() const override {
            return m_debugMode;
        }


        GLuint VAO, VBO;
    private:
        int m_debugMode;
        const char* vertexShaderSource = R"(
 #version 330 core

precision mediump float;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normals;
out vec3 fColor; // Output color
out vec3 Normal;  // Output normal in world space
out vec4 fragPosition;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int applyModelTransform;
void main()
{
 Normal = normals; // Pass normals to the fragment shader
   // fragPosition = projection * vec4(position,1.0f); 

 if (applyModelTransform == 1) {
        gl_Position = projection * view * model * vec4(position, 1.0f);
   fColor = color; // Pass color directly to the fragment shader
 fragPosition = projection * vec4(position, 1.0f);
}
else{
    gl_Position = projection * view * vec4(position, 1.0f);
    fColor = color; // Pass color directly to the fragment shader
}
}
    )"; 

        const char* fragmentShaderSource = R"(#version 330 core

in vec3 fColor; // Input color from vertex shader
in vec3 Normal; // Input normal in world space from vertex shader
out vec4 FragColor;
in vec4 fragPosition;
uniform vec3 ambientColor; // Ambient light color
uniform float time;

// Define light properties
vec3 lightDirection = normalize(vec3(0.0f, -1.0f, 0.0f)); // Sun direction (adjust as needed)
vec3 diffuseColor = vec3(1.0f, 1.0f, 1.0f); // Diffuse light color

// Define terrain color thresholds
uniform float waterThreshold;
uniform float grassThreshold;
uniform float rockyThreshold;
uniform float snowThreshold;
uniform vec3 waterColor;
uniform vec3 grassColor;
uniform vec3 rockyColor;
uniform vec3 snowColor;

void main()
{
    // Calculate the height of the terrain at the fragment position
    float terrainHeight = fragPosition.y;

    // Initialize the result color
    vec3 resultColor = vec3(0.0);

    // Check the height of the terrain and apply color thresholds
    if (terrainHeight < waterThreshold) {
        resultColor = waterColor; // Set water color
    } else if (terrainHeight < grassThreshold) {
        resultColor = grassColor; // Set grass color
    } else if (terrainHeight < rockyThreshold) {
        resultColor = rockyColor; // Set rocky color
    } else if (terrainHeight > snowThreshold) {
        resultColor = snowColor; // Set snow color
    } else {
        resultColor = rockyColor; // Default to rocky color for heights above snow threshold
    }

    // Calculate the intensity of the diffuse light (Lambertian shading)
    float diffuseIntensity = max(dot(Normal, lightDirection), 0.0f);

    // Combine the vertex color with ambient and diffuse lighting
    vec3 ambient = ambientColor * resultColor;
    vec3 diffuse = diffuseIntensity * diffuseColor * resultColor;

    // Final color output
    vec3 finalColor = ambient + diffuse;

    FragColor = vec4(finalColor, 1.0f);
}


    )";
       

    };

