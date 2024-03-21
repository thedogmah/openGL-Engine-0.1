//#pragma once




BT_USE_DOUBLE_PRECISION

#include <c:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\LinearMath/btIDebugDraw.h>
#include <c:\Users\ryanb\vcpkg\packages\bullet3_x64-windows\include\bullet\LinearMath/btVector3.h>
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

struct TerrainInfo {
    int isWater;
    int isMountain;
    int isForest;
    int isDesert;
};


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec2 uvs;
layout(location = 4) in int isWater;     // Attribute location for isWater
layout(location = 5) in int isMountain;  // Attribute location for isMountain
layout(location = 6) in int isForest;    // Attribute location for isForest
layout(location = 7) in int isDesert; 
layout(location = 8) in float verticesID; //Sending unique vertices ID for each traingle already
layout(location = 9) in vec3 pickedRGB;

 out float verticesUniqueID;//unqiue IDs for terrain
out vec3 fColor; // Output color
out vec3 Normal;  // Output normal in world space
out vec4 fragPosition;
flat out vec3 vecIDs;//for vertice ID XZ coords;
out float modifiedY;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

flat out vec3 pickedRGBData;

uniform int applyModelTransform;
uniform float heightScale; // Scale factor for height map
uniform float heightOffset; // Offset to control base height
uniform int riverPathIndices[5408];
uniform float riverBed;
uniform sampler2D detailMap;
out vec2 fragResolution;
uniform float time;
out vec2 uvsOut;
uniform sampler2D mudHeight;
uniform sampler2D mudNormals;
out vec2 TexCoord;  // Output texture coordinates for fragment shader
uniform vec4 plane;
flat out int isRiverVertex;



void main()
{

pickedRGBData = pickedRGB;
verticesUniqueID =verticesID;
fragResolution = vec2(2560.0, 1440.0);
modifiedY = 0;
  isRiverVertex = 0;
  vec4 worldPosition = model * vec4(position.x, position.y, position.z, 1.0);
vec4 clipspace =  projection * view * model * vec4(position, 1.0f);
vec4 IDpass = projection * view * model * vec4(position, 1.0f);

vecIDs.x = position.x;
vecIDs.y = position.y;
vecIDs.z = position.z;

    // Set the clip distance based on the world position
    gl_ClipDistance[0] = dot(worldPosition, plane);

if(isWater >0)
    {
       isRiverVertex = 1;
     // Wave the river vertices based on time
        gl_Position = vec4(position.x, position.y, position.z, 1.0);
        TexCoord = uvs;
    } 

 Normal = normals; // Pass normals to the fragment shader
Normal = normalize(normals);

// Sample mud normals
vec3 mudNormalsSample = normalize(texture(mudNormals, uvs.xy).xyz * 2.0 - 1.0);

// Modify normals using mud normals

Normal = normalize(normals + mudNormalsSample);

uvsOut = uvs;
   // fragPosition = projection * vec4(position,1.0f); 
float mudDisplacement = texture(mudHeight, uvs.xy).r * 2.0;

 if (applyModelTransform == 1) {

     float heightMapValue = texture(detailMap, uvs).r;

        // Calculate displacement based on the heightmap
        float displacement = (heightMapValue * heightScale) + heightOffset;

        // Apply displacement only to the Y-coordinate of the vertex position
        vec3 displacedPosition = position + vec3(0.0, mudDisplacement, 0.0) ;
  if (isRiverVertex == 1) {
       displacedPosition.y -= riverBed; // Adjust this value based on your requirements
        modifiedY = displacedPosition.y;         
   vec3 displacedNormal = normalize(mat3(transpose(inverse(model))) * normals);

        // Pass the modified normal to the fragment shader
        Normal = displacedNormal;

}
        // Transform the displaced vertex position
        gl_Position = projection * view * model * vec4(displacedPosition, 1.0f);
        fragPosition = projection * vec4(displacedPosition, 1.0f);
        fColor = color;

    //    gl_Position = projection * view * model * vec4(position, 1.0f);
 //  fColor = color; // Pass color directly to the fragment shader
// fragPosition = projection *  vec4(position, 1.0f);
}
else{


    // Apply displacement only to the Y-coordinate of the vertex position
    vec3 displacedPosition = position + vec3(0.0, -mudDisplacement, 0.0);

    gl_Position = projection * view * vec4(displacedPosition, 1.0f);
    fragPosition = projection * vec4(displacedPosition, 1.0f);
    fColor = color;

 //   gl_Position = projection * view * vec4(position, 1.0f);
   // fColor = color; // Pass color directly to the fragment shader
}
}
    )"; 

        const char* fragmentShaderSource = R"(#version 330 core

in vec3 fColor; // Input color from vertex shader
in vec3 Normal; // Input normal in world space from vertex shader
in vec2 uvsOut; // UV coordinates from vertex shader
flat in vec3 vecIDs;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColor2;


//mouse picking RGB data
flat in vec3 pickedRGBData;

in vec2 fragResolution;
in vec4 fragPosition;

 in float verticesUniqueID; //id for vertices from shader for selecting terrain
vec2 iResolution = vec2(2560.0, 1440.0);

flat in int isRiverVertex; 
uniform vec3 ambientColor; // Ambient light color
uniform float time;
in float modifiedY; 
uniform sampler2D txGrass;
uniform sampler2D txHighGrass;
uniform sampler2D txRock;

uniform sampler2D txHighRock;
uniform sampler2D txPeak;
uniform sampler2D normalMap;
uniform sampler2D detailMap;
uniform sampler2D heightMap;

uniform sampler2D waterTexture;
uniform sampler2D mudTexture; 

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
uniform float slopeThreshold;
uniform int useNormalMap;
uniform int useDetailMap;
uniform float radian;
uniform int gridSize;
//edit terrain mode switch
uniform int terrainEditMode;

// Define shininess factor for specular reflection (added)
uniform float shininess;

float avg(vec4 color) {
    return (color.r + color.g + color.b)/3.0;
}


void main()
{
    vec3 normalMapSample = texture(normalMap, uvsOut).xyz;
    // Apply normal map to modify the normal vector
    vec3 modifiedNormal = normalize(normalMapSample * 2.0 - 1.0);
    vec3 detailMapSample = texture(detailMap, uvsOut).xyz;
    float detailMapValue = texture(detailMap, uvsOut).r;
    vec3 mudColor = texture(mudTexture, uvsOut * 5).rgb;

    // Calculate the dot product between the normal vector and the up vector
    float slopeFactor = dot(modifiedNormal, vec3(0.0, 1.0, 0.0)) + modifiedY;

    // Define a threshold for the slope at which mud starts appearing


    // Blend based on slope and detail map value
    float blendFactor = smoothstep(slopeThreshold - 0.4, slopeThreshold, slopeFactor);

    // Preserve the original sampled color before height-based conditions
    vec3 originalColor = vec3(0.0);

    // Height-based conditions to update originalColor
    float terrainHeight = fragPosition.y + modifiedY;

    if (terrainHeight < waterThreshold) {
        originalColor = texture(txGrass, uvsOut * 5).rgb;
    }
    else if (terrainHeight >= waterThreshold && terrainHeight < grassThreshold) {
        float factor = smoothstep(waterThreshold, waterThreshold + smoothStepFactor, terrainHeight);
        originalColor = mix(texture(txGrass, uvsOut *5).rgb, texture(txHighGrass, uvsOut *5).rgb, factor);
    }
    else if (terrainHeight >= grassThreshold && terrainHeight < rockyThreshold) {
        float factor = smoothstep(grassThreshold, grassThreshold + smoothStepFactor, terrainHeight);
        originalColor = mix(texture(txHighGrass, uvsOut * 5).rgb, texture(txRock, uvsOut * 5).rgb, factor);
    }
    else if (terrainHeight >= rockyThreshold && terrainHeight < snowThreshold) {
        float factor = smoothstep(rockyThreshold, rockyThreshold + smoothStepFactor, terrainHeight);
        originalColor = mix(texture(txRock, uvsOut * 5).rgb, texture(txHighRock, uvsOut * 5).rgb, factor);
    }
    else if (terrainHeight >= snowThreshold && terrainHeight < peakThreshold) {
        float factor = smoothstep(snowThreshold, snowThreshold + smoothStepFactor, terrainHeight);
        originalColor = mix(texture(txHighRock, uvsOut * 5).rgb, texture(txPeak, uvsOut * 5).rgb, factor);
    }
    else if (terrainHeight >= peakThreshold) {
        originalColor = texture(txPeak, uvsOut * 5).rgb;
    }

    // Blend colors using the mud color and detail map value

    vec3 sampledColor = originalColor;
    // Check the slope factor for blending
    if (slopeFactor < slopeThreshold) {
        // Use mud color for steep slopes
        sampledColor = mix(originalColor, mudColor, blendFactor);
    }
    else {
        // Use the original color for not steep slopes
        sampledColor = mix(mudColor, originalColor, blendFactor);
    }



    // Calculate the sun's direction based on spherical coordinates and time
    float sunAltitude = radians(radian);
    float sunAzimuth = radians(180.0); // You can adjust this based on your scene's orientation
    vec3 sunDirection = vec3(cos(sunAzimuth) * sin(sunAltitude), cos(sunAltitude), sin(sunAzimuth) * sin(sunAltitude));

    // Calculate the sun's color based on time of day
    vec3 sunColor = diffuseColor;

    // Calculate the intensity of the sun's light
    float sunIntensity = max(dot(normalMapSample, -sunDirection), 0.0); // Lambertian shading

    // Use lightDirection, diffuseColor, and diffuseIntensity for sun-related variables
    vec3 sunAmbient = (ambientColor * 0.5) * diffuseColor; // Keep diffuseColor for ambient lighting
    vec3 sunDiffuse = (sunIntensity * sunBrightness) * sunColor; // Use sunColor instead of diffuseColor for sunlight

    // Calculate the reflection vector for the specular component
    vec3 viewDirection = normalize(-fragPosition.xyz);
    vec3 reflectDirection = reflect(-sunDirection, Normal);

    // Calculate the specular intensity (Phong reflection model)
    float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);

    // Combine the sun's color and intensity with your existing lighting model
    vec3 ambient = (ambientColor * 0.4);
    vec3 diffuse = (sunIntensity * sunBrightness) * sunColor; // Use sunColor for sunlight and apply sampled texture color
    vec3 specular = (specularIntensity * 0.05) * specularColor; // Use specular color directly

    // Final color output
    vec3 blendedColor = mix(sampledColor, mudColor, smoothstep(slopeThreshold - 0.6, slopeThreshold, slopeFactor));

    vec3 finalColor = ambient + diffuse + specular;;

    // Untag for HRD range method - Reinhard method. (balances ultra-bright colors)
    // finalColor = finalColor / (finalColor + vec3(1.0)); // Simple tone mapping operator (Reinhard tone mapping)
    if (useDetailMap == 1) {
         FragColor = vec4(0.0,verticesUniqueID,0.0, 0.5);
            }
    
    else 
            {
            vec4 finalColors = mix(vec4(finalColor * blendedColor, verticesUniqueID), vec4(verticesUniqueID, verticesUniqueID,verticesUniqueID,verticesUniqueID), 0.3);
            vec4 blended = mix(vec4(finalColor,0.0), vec4(verticesUniqueID, verticesUniqueID,verticesUniqueID, 0.0), 0.5);
            FragColor = vec4(finalColors);
            }
  vec3 waterColor = vec3(0.0, 0.5, 0.9); // Adjust the RGB values for the water color
    float transparency = 0.7; // Adjust the transparency level

   if (isRiverVertex > 0) {
        // Apply wavy effect to the texture coordinates
        vec2 wavyCoords = uvsOut + vec2(0.0, sin(time * 2.0 + uvsOut.x * 5.0) * 0.1);
        
        // Sample the water texture using the wavy coordinates
        FragColor = texture(waterTexture, wavyCoords);

}



if (terrainEditMode == 1)
{

FragColor2 = vec4(vecIDs.x,vecIDs.y,vecIDs.z,0.0);

FragColor = vec4(
vecIDs.x/128,
0.0,
vecIDs.z/128, 0.0);

if (int(vecIDs.x / 127.0 * 256.0)>= int(pickedRGBData.x) && 
    int(vecIDs.z / 127.0 * 256.0) >= int(pickedRGBData.z) &&
int(vecIDs.x / 127.0 * 256.0)<= int(pickedRGBData.x +50) && 
    int(vecIDs.z / 127.0 * 256.0) <= int(pickedRGBData.z+50)

)
 {
    FragColor = vec4(0.2, 1.0, 0.5, 0.1);
}
//else
//if (int(vecIDs.x / 128.0 * 255.0) == int(pickedRGBData.x) && 
//    int(vecIDs.z / 128.0 * 255.0) == int(pickedRGBData.z))
// {
//    FragColor = vec4(0.2, 1.0, 0.5, 0.1);
//}



//if(vecIDs.x == pickedRGBData.x && vecIDs.z == pickedRGBData.z)
//{
//FragColor = vec4(0.1,1.0,0.5,0.0);
//}
//cannot divide a
}

}

    )";
       

    };


    //if (isRiverVertex == 1) {
    //    FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green color for river path fragments
    //}
    //if (isRiverVertex == 2) {
    //    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color for non-river path fragments
    //}
    //if (isRiverVertex == 0) {
    //    FragColor = vec4(1.0, 0.0, 1.0, 1.0); // Red color for non-river path fragments
    //}
   