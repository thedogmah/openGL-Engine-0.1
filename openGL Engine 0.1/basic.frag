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


    vec3 textureColor = vec3(1.0); // Default color if no texture is present
if (texture(texture1, textureCoords).r > 0.0 || texture(texture1, textureCoords).g > 0.0 || texture(texture1, textureCoords).b > 0.0)
{
    textureColor = texture(texture1, textureCoords).rgb; // Sample texture color
}

vec3 diffuse = material.diffuse;

if (texture(texture1, textureCoords).r > 0.0 || texture(texture1, textureCoords).g > 0.0 || texture(texture1, textureCoords).b > 0.0) {
    diffuse *= textureColor; // Multiply by texture color only when the texture is present
}

//        vec3 diffuse = material.diffuse * texture(texture1, textureCoords).rgb; // Diffuse



//array size below comes from an obscure variable (sceneLights.size()

 for (int i = 0; i < arraySize; i++) {
    vec3 sunDir = normalize(lights[i].position - FragPosWorld);
    float diff = max(dot(normalize(Normal), sunDir), 0.0); // Diffuse

    // Ambient
    vec3 ambient = material.ambient * lights[i].colour * lights[i].strength;

    // Specular
    vec3 specular = vec3(0.0);
    if (diff > 0.0) {
        vec3 viewDir = normalize(viewPos - FragPosWorld);
        vec3 halfwayDir = normalize(sunDir + viewDir);
        float spec = pow(max(dot(normalize(Normal), halfwayDir), 0.0), material.shininess);
        specular = material.specular * lights[i].colour * lights[i].strength * spec;
    }

    vec3 diffuseLight = diff * lights[i].colour * lights[i].strength * diffuse;

    if (texture(texture1, textureCoords).r > 0.0 || texture(texture1, textureCoords).g > 0.0 || texture(texture1, textureCoords).b > 0.0) {
        diffuseLight *= texture(texture1, textureCoords).rgb; // Multiply by texture color only when the texture is present
        specular *= texture(texture1, textureCoords).rgb;
    }

    // Combine ambient, diffuse, and specular lighting
    vec3 result = ambient + diffuseLight + specular;

    FragColorOutput += vec4(result, 1.0);
}
    }
}   