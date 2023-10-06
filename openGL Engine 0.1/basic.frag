#version 450 core

//Light structure for the SSBO data from cpp
struct Light {
    vec3 position;
    float strength;
    vec3 colour;
    vec3 startingPosition;
    
}; 

//Material structure for SSBO data
struct Material {
    vec3 ambient;
    float shininess;
    vec3 diffuse;
    float transparency;
    vec3 specular;
      
};



layout (std430, binding = 0) buffer LightsBuffer{

Light lights[5];

};

layout (std430, binding = 1) buffer MaterialBuffer {
   Material material;
};

in vec2 textureCoords;
in vec3 Normal;
in vec3 FragPosWorld;
in vec3 FragColor; // receive color from vertex shader
uniform sampler2D texture1;
uniform vec3 viewPos; // Camera position    
uniform float exposure; // exposure for correcting HDR values
uniform int mousePicking;
in vec3 mouseColour; // comes from SSBO in vertex
out vec4 FragColorOutput;
vec4 resultWithAlpha = vec4(0.0);

uniform int arraySize;

void main() {
    if (mousePicking == 1) {
        FragColorOutput = vec4(mouseColour, 1.0);
    } else {
        vec3 diffuse = material.diffuse * texture(texture1, textureCoords).rgb; // Diffuse

        for (int i = 0; i < arraySize; i++) {
            // Calculate the direction from the fragment to the light
            vec3 sunDir = normalize(lights[i].position - FragPosWorld);
            float diff = max(dot(normalize(Normal), sunDir), 0.0); // Diffuse
            vec3 diffuseLight = lights[i].colour * lights[i].strength * material.diffuse * diff * texture(texture1, textureCoords).rgb;

            // Combine diffuse with sunlight
            vec3 result = diffuseLight;

            FragColorOutput += vec4(result, 1.0);
        }
    }
}   