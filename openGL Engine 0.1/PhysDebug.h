#pragma once

#define BT_USE_DOUBLE_PRECISION

#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btVector3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "character.h"


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
        int viewLocation =0;
            int projectionLocation=0;//These were initially declared in the setMatrices (CPU optomisedation)
        int m_debugMode;
        const char* vertexShaderSource = R"(
 #version 330 core

precision mediump float;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec2 uvs;
out vec3 fColor; // Output color
out vec3 Normal;  // Output normal in world space
out vec4 fragPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int applyModelTransform;

out vec2 uvsOut;
void main()
{

 Normal = normals; // Pass normals to the fragment shader
Normal = normalize(normals);
uvsOut = uvs;
   // fragPosition = projection * vec4(position,1.0f); 

 if (applyModelTransform == 1) {
        gl_Position = projection * view * model * vec4(position, 1.0f);
   fColor = color; // Pass color directly to the fragment shader
 fragPosition = projection *  vec4(position, 1.0f);
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
in vec2 uvsOut; // UV coordinates from vertex shader
out vec4 FragColor;
in vec4 fragPosition;
uniform vec3 ambientColor; // Ambient light color
uniform float time;

uniform sampler2D txGrass;
uniform sampler2D txHighGrass;
uniform sampler2D txRock;
uniform sampler2D txHighRock;
uniform sampler2D txPeak;


//WORLD UNIFORMS



// Define light properties
uniform vec3 lightDirection; // Sun direction (adjust as needed)
uniform vec3 diffuseColor; // Diffuse light color
uniform vec3 specularColor; // Specular light color (added)
uniform float sunBrightness;


// Define terrain height thresholds
uniform float waterThreshold = 10;
uniform float grassThreshold = 20;
uniform float rockyThreshold = 30;
uniform float snowThreshold = 40;
uniform float peakThreshold = 60;
uniform float smoothStepFactor;
// Define shininess factor for specular reflection (added)
uniform float shininess;
void main()
{
    // Calculate the height of the terrain at the fragment position
    float terrainHeight = fragPosition.y;

    // Initialize the result color
    vec3 resultColor = vec3(0.0);

 vec3 sampledColor = vec3(0.0);

if (terrainHeight < waterThreshold) {
    sampledColor = texture(txGrass, uvsOut).rgb;
    } 

else if (terrainHeight >= waterThreshold && terrainHeight < grassThreshold) {
    
    float blendFactor = smoothstep(waterThreshold, waterThreshold + smoothStepFactor, terrainHeight);
    sampledColor = mix(texture(txGrass, uvsOut).rgb, texture(txHighGrass, uvsOut).rgb, blendFactor);
} 

else if (terrainHeight >= grassThreshold && terrainHeight < rockyThreshold) {
   
     float blendFactor = smoothstep(grassThreshold, grassThreshold +smoothStepFactor, terrainHeight);
    sampledColor = mix(texture(txHighGrass, uvsOut).rgb, texture(txRock, uvsOut).rgb, blendFactor);
}

else if (terrainHeight >= rockyThreshold && terrainHeight < snowThreshold) {
    
    float blendFactor = smoothstep(rockyThreshold, rockyThreshold + smoothStepFactor, terrainHeight);
    sampledColor = mix(texture(txRock, uvsOut).rgb, texture(txHighRock, uvsOut).rgb, blendFactor);
}

else if (terrainHeight >= snowThreshold && terrainHeight < peakThreshold) {
    
    float blendFactor = smoothstep(snowThreshold, snowThreshold + smoothStepFactor, terrainHeight);
    sampledColor = mix(texture(txHighRock, uvsOut).rgb, texture(txPeak, uvsOut).rgb, blendFactor);
}

else if (terrainHeight >= peakThreshold){

    // No need for blending, just use the peak texture directly
    sampledColor = texture(txPeak, uvsOut).rgb;

   /*  float blendFactor = smoothstep(peakThreshold, peakThreshold + smoothStepFactor, terrainHeight);
    sampledColor = mix(texture(txHighRock, uvsOut).rgb, texture(txPeak, uvsOut).rgb, blendFactor);*/
}
    

// Calculate the sun's direction based on spherical coordinates and time
float sunAltitude = radians(90.0 - ((time / 24.0) * 180.0)); // Normalize time to 0-1 and convert to degrees

float sunAzimuth = radians(180.0); // You can adjust this based on your scene's orientation
vec3 sunDirection = vec3(cos(sunAzimuth) * sin(sunAltitude), cos(sunAltitude), sin(sunAzimuth) * sin(sunAltitude));

// Calculate the sun's color based on time of day
vec3 sunColor =diffuseColor;// mix(vec3(1.0, 0.8, 0.6), vec3(1.0, 1.0, 1.0), abs(sunAltitude) / radians(45.0)); // Warm colors at sunrise/sunset, white at midday

// Calculate the intensity of the sun's light
float sunIntensity = max(dot(Normal, -sunDirection), 0.0); // Lambertian shading

// Use lightDirection, diffuseColor, and diffuseIntensity for sun-related variables
vec3 sunAmbient = (ambientColor * 0.5) * diffuseColor; // Keep diffuseColor for ambient lighting
vec3 sunDiffuse = (sunIntensity * sunBrightness) * sunColor; // Use sunColor instead of diffuseColor for sunlight

   
    // Calculate the reflection vector for the specular component
    vec3 viewDirection = normalize(-fragPosition.xyz);
    vec3 reflectDirection = reflect(-sunDirection, Normal);

    // Calculate the specular intensity (Phong reflection model)
    float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);

 // Combine the sun's color and intensity with your existing lighting model
vec3 ambient = (ambientColor * 0.4) * sampledColor;
vec3 diffuse = (sunIntensity * sunBrightness) * sunColor * sampledColor; // Use sunColor for sunlight and apply sampled texture color
vec3 specular = (specularIntensity * 0.05) * specularColor; // Use specular color directly



    // Final color output
    vec3 finalColor = ambient + diffuse + specular;
 
//untag for HRD range method - Reinhard method. (balances ultra bright colours
// finalColor = finalColor / (finalColor + vec3(1.0)); // Simple tone mapping operator (Reinhard tone mapping)

    FragColor = vec4(finalColor, 1.0f);
}
    )";
       

    };
