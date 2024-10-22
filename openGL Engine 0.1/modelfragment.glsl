#version 450 core

out vec4 FragColor;

in vec2 TexCoords;          // Texture coordinates from vertex shader
in vec3 FragNormal;         // Normal from vertex shader
in vec3 FragPos;            // Fragment position in world space

uniform sampler2D texture_diffuse;    // Diffuse texture
uniform sampler2D texture_normal;     // Normal map texture
uniform vec3 viewPos;                 // Camera/view position

// Sunlight uniforms
uniform vec3 sunDiffuseColor;         // Sunlight color
uniform float sunBrightness;          // Sunlight intensity/brightness
uniform float radiansTime;            // Time for sun direction calculation

// Material struct
struct Material {
    vec3 diffuseColor;   // Diffuse color of the material
    vec3 specularColor;  // Specular color of the material
    vec3 emissiveColor;  // Emissive color of the material
    float shininess;     // Shininess factor for specular highlights
    float transparency;  // Transparency factor (if applicable)
    float indexOfRefraction; // Index of refraction (if applicable)
    int illuminationModel;   // Illumination model to be used (if applicable)
};

// Define material
uniform Material material;

// Dynamic lights SSBO
struct Light {
    vec3 position;           // Same as on the CPU side (12 bytes)
    float strength;          // Renamed from "intensity" to match CPU side (4 bytes)

    vec3 colour;             // Renamed from "color" to match CPU side (12 bytes)
    float pad1;              // Padding to align the next vec3 (4 bytes)


    vec3 direction;          // Direction for directional lights
    float isDirectional;   // no BOOLS in shaders, so we use a float check (should be an int though ;)


    vec3 startingPosition;   // Added to match CPU side (12 bytes)
    float pad2;              // Padding for alignment (4 bytes)
};


layout(std430, binding = 0) buffer LightsBuffer {
    Light lights[];   // Array of dynamic lights from the SSBO
};

uniform int arraySize; // Number of active dynamic lights in the SSBO

void main() {
    // --- Texture sampling ---
    vec4 texDiffuseColor = texture(texture_diffuse, TexCoords); // Sample the diffuse texture
    vec3 texNormal = texture(texture_normal, TexCoords).rgb;    // Sample the normal map texture
    
    // Convert normal from [0,1] range to [-1,1] range
    vec3 normalMap = normalize(texNormal * 2.0 - 1.0);  // Convert to [-1, 1] range
    vec3 adjustedNormal = normalize(FragNormal + normalMap * 0.5); // Blend fragment normal with bump map normal

    // --- Sunlight calculation ---
    // Calculate sun direction using radiansTime
    float sunAltitude = radians(radiansTime);
    float sunAzimuth = radians(180.0);

    vec3 sunDirection = vec3(
        cos(sunAzimuth) * sin(sunAltitude),
        cos(sunAltitude),
        sin(sunAzimuth) * sin(sunAltitude)
    );

    // Diffuse lighting from sunlight
    float sunDiffuseFactor = max(dot(adjustedNormal, sunDirection), 0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 sunReflectDir = reflect(-sunDirection, adjustedNormal);
    float sunSpecFactor = pow(max(dot(viewDir, sunReflectDir), 0.0), material.shininess);

    // Sunlight contributions
    vec3 sunDiffuse = sunDiffuseFactor * sunDiffuseColor * texDiffuseColor.rgb * material.diffuseColor * sunBrightness;
    vec3 sunSpecular = sunSpecFactor * sunDiffuseColor * sunBrightness * material.specularColor;

    // --- Dynamic lights calculation (SSBO) ---
    vec3 dynamicLightDiffuse = vec3(0.0);
    vec3 dynamicLightSpecular = vec3(0.0);
    
    for (int i = 0; i < arraySize; i++) {
        float attenuation = 1.0;  // Default to no attenuation for directional lights
        vec3 lightDir;
 
    if (lights[i].isDirectional == 0.0) {
             lightDir = normalize(lights[i].position - FragPos);
           float distance = length(lights[i].position - FragPos);
            attenuation = 100.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
            }
    else{
             // --- Directional light logic with positional falloff ---
            lightDir = normalize(-lights[i].direction);
            
            // Compute the vector from the light's position to the fragment
            vec3 lightToFrag = FragPos - lights[i].position;
            float distance = length(lightToFrag);
            
            // Use attenuation for directional lights based on distance
            // Assuming the light affects objects within a certain distance
            float maxRange = 100.0;  // Set the maximum effective range for the directional light
            attenuation = max(0.0, 1.0 - (distance / maxRange));

            // Directional light angle (to create a cone effect)
            float cutoffAngle = cos(radians(45.0));  // Define cutoff angle (45 degrees)
            float theta = dot(normalize(lightToFrag), lightDir);

            if (theta < cutoffAngle) {
                attenuation = 0.0;  // Ignore fragments outside the light cone
            }
                }
        float diff = max(dot(adjustedNormal, lightDir), 0.0);
        vec3 reflectDir = reflect(-lightDir, adjustedNormal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     


vec3 lightDiffuse = diff * lights[i].colour * lights[i].strength * material.diffuseColor * attenuation;
vec3 lightSpecular = spec * lights[i].colour * lights[i].strength * material.specularColor * attenuation;

       

        dynamicLightDiffuse += lightDiffuse;
        dynamicLightSpecular += lightSpecular;
    }

    // --- Final color calculation ---
    vec3 ambient = vec3(0.35, 0.35, 0.35) * texDiffuseColor.rgb; // Simple ambient lighting
    vec3 finalDiffuse = sunDiffuse + dynamicLightDiffuse;
    vec3 finalSpecular = sunSpecular + dynamicLightSpecular;

    vec3 finalColor = ambient + finalDiffuse + finalSpecular + material.emissiveColor;

    // Output the final color
    FragColor = vec4(finalColor, material.transparency);
}
