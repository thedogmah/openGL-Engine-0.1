#version 450 core

out vec4 FragColor;

in vec2 TexCoords;          // Texture coordinates from vertex shader
in vec3 FragNormal;         // Normal from vertex shader
in vec3 FragPos;            // Fragment position in world space

uniform sampler2D texture_diffuse;    // Diffuse texture
uniform sampler2D texture_normal;     // Normal map texture
uniform vec3 viewPos;                 // Camera/view position

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

uniform Material material;
uniform float sunBrightness;          // Brightness of sun light
uniform vec3 sunDiffuseColor;         // Sun light color
uniform float radiansTime;            // Time for sun direction calculation

void main() {
    // Sample textures
    vec4 texDiffuseColor = texture(texture_diffuse, TexCoords); // Sample the diffuse texture
    vec3 texNormal = texture(texture_normal, TexCoords).rgb;    // Sample the normal map texture
    
    // Convert normal from [0,1] range to [-1,1] range
    vec3 normalMap = normalize(texNormal * 2.0 - 1.0);  // Convert to [-1, 1] range

    // We should now blend this normal map with the fragment normal (which is typically in world space)
    // but in this case, you are directly using FragNormal in world space
    // Ideally, you'd transform the normal map from tangent space to world space, but skipping that:
    vec3 adjustedNormal = normalize(FragNormal + normalMap * 0.5); // Blend fragment normal with bump map normal

    // Calculate sun direction using radiansTime
    float sunAltitude = radians(radiansTime);
    float sunAzimuth = radians(180.0);

    vec3 sunDirection = vec3(
        cos(sunAzimuth) * sin(sunAltitude),
        cos(sunAltitude),
        sin(sunAzimuth) * sin(sunAltitude)
    );

    // Basic diffuse lighting (with bump-mapped normal)
    float dirDiff = max(dot(adjustedNormal, sunDirection), 0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 dirReflectDir = reflect(-sunDirection, adjustedNormal);
    float dirSpec = pow(max(dot(viewDir, dirReflectDir), 0.0), material.shininess);

    // Ambient light contribution
    vec3 ambientColor = vec3(0.1, 0.1, 0.1);
    vec3 ambient = ambientColor * texDiffuseColor.rgb;

    // Minimum diffuse lighting factor
    float minimumDiffuse = 0.2;
    float effectiveDiffuse = max(dirDiff, minimumDiffuse);

    // Final diffuse and specular contributions
    vec3 diffuse = effectiveDiffuse * sunDiffuseColor * texDiffuseColor.rgb * material.diffuseColor * sunBrightness;
    vec3 specular = dirSpec * sunDiffuseColor * sunBrightness * material.specularColor; // Material's specular color

      // --- Emissive Lighting ---
    vec3 emissive = material.emissiveColor;  // Emissive color does not depend on light sources

    // Combine all components into the final color
    vec3 finalColor = ambient + diffuse + specular + emissive;

   

    // Output the final color
    FragColor = vec4(finalColor, material.transparency);
}
