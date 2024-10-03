#version 450 core
out vec4 FragColor;

in vec2 TexCoords;  // Texture coordinates from vertex shader
in vec3 FragNormal; // Normal from vertex shader
in vec3 FragPos;    // Position in world space

uniform sampler2D texture_diffuse1; // Diffuse texture
uniform sampler2D texture_specular1; // Specular texture

// Hard-coded light direction (example: pointing downwards)
const vec3 lightDirection = vec3(0.2, 2.0, 0.3); // Replace this with your desired light direction
uniform vec3 viewPos; // Camera position


// Define ambient color (purple)
const vec3 ambientColor = vec3(0.2, 0.2, 0.2); // Purple ambient light



void main() {
    // Sample the texture colors
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    vec4 specColor = texture(texture_specular1, TexCoords); // Sample specular texture

    // Normalize the fragment normal
    vec3 norm = normalize(FragNormal);

    // Calculate the light intensity based on the angle between the light direction and the normal
    float diff = max(dot(norm, normalize(lightDirection)), 0.0);

    // Specular lighting calculations
    // Calculate the view direction
    vec3 viewDir = normalize(viewPos - FragPos); // Direction from fragment to camera
    // Calculate reflection direction
    vec3 reflectDir = reflect(-normalize(lightDirection), norm);
    // Calculate specular component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128); // 256 is the shininess factor

    // Combine diffuse and specular colors
    vec3 diffuseColor = diff * texColor.rgb; // Diffuse color contribution

    // Adjust specular color contribution based on the specular texture
    vec3 specularColor = specColor.rgb * spec * 10; // Specular color contribution

    vec3 ambientLight = ambientColor * texColor.rgb; // Ambient light contribution
     ambientLight *=0.2;
    // Final color output - preserve the alpha from the texture
    FragColor = vec4(ambientLight + diffuseColor + specularColor, texColor.a);
}
