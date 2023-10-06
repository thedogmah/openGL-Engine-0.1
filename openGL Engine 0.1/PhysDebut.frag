#version 330 core

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

// Define light properties
uniform vec3 lightDirection; // Sun direction (adjust as needed)
uniform vec3 diffuseColor; // Diffuse light color
uniform vec3 specularColor; // Specular light color (added)

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
    
    // Calculate the intensity of the diffuse light (Lambertian shading)
    float diffuseIntensity = max(dot(Normal, lightDirection), 0.0f);

    // Calculate the reflection vector for the specular component
    vec3 viewDirection = normalize(-fragPosition.xyz);
    vec3 reflectDirection = reflect(-lightDirection, Normal);

    // Calculate the specular intensity (Phong reflection model)
    float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);

    // Combine the sampled texture color with ambient, diffuse, and specular lighting
    vec3 ambient = (ambientColor * 0.4) * sampledColor;
    vec3 diffuse = (diffuseIntensity * 0.4) * diffuseColor * sampledColor;
    vec3 specular = (specularIntensity * 0.05) * specularColor ; // Use specular color directly

    // Final color output
    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0f);
}