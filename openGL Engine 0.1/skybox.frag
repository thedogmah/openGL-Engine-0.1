#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 sunColor; 
float sunFactor; // Value between 0.0 (no sun color) and 1.0 (full sun color)

void main()
{
    
    // Sample the skybox color
    vec4 skyboxColor = texture(skybox, TexCoords);
    sunFactor =0.4;
    // Blend the sunColor with the skybox color using sunFactor
    vec4 finalColor = mix(skyboxColor, skyboxColor * vec4(sunColor, 1.0), sunFactor);

    // Set the final fragment color
    FragColor = finalColor;
}
