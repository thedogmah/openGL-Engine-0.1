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

uniform sampler2D texture1;
uniform vec3 viewPos; // Camera position
uniform float exposure; // exposure for correcting HDR values
uniform int mousePicking;
in vec3 mouseColour; // comes from SSBO in vertex
out vec4 FragColor;
vec4 resultWithAlpha = vec4(0.0);

uniform int arraySize;

void main() {
   if (mousePicking == 1)
   {
    FragColor = vec4(mouseColour, 1.0);

   }
   else
   {
  vec3 ambient = material.ambient * texture(texture1, textureCoords).rgb; // Ambient

for (int i = 0; i < arraySize; i++)
{
    // Calculate the direction from the fragment to the light
    vec3 sunDir = normalize(lights[i].position - FragPosWorld);
      vec3 viewDir = normalize(viewPos - FragPosWorld); // Direction to the viewer (camera)
    float dotProduct = dot(normalize(Normal), sunDir);

    vec3 sunAmbient = lights[i].colour * lights[i].strength * max(dotProduct, 0.0);

    float diff = max(dot(normalize(Normal), sunDir), 0.0);    // Diffuse
    vec3 diffuse = lights[i].colour * lights[i].strength * material.diffuse * diff * texture(texture1, textureCoords).rgb ;

      // Specular lighting
          vec3 halfwayDir = normalize(sunDir + viewDir);

        float spec = pow(max(dot(Normal, halfwayDir), 0.0), material.shininess);
        vec3 specular = lights[i].colour * lights[i].strength * material.specular * spec;


    // Combine ambient and diffuse with sun's position influence
    vec3 result = sunAmbient + diffuse + specular;

    resultWithAlpha += vec4(result, material.transparency);
}

resultWithAlpha += vec4(ambient, 0.0);
//FragColor = vec4(resultWithAlpha);
  // FragColor = vec4(result, 1.0);
   //  const float gamma = 2.2;
   // vec3 hdrColor = resultWithAlpha.rgb;
  
    // exposure tone mapping
  //  vec3 mapped = vec3(1.0) - exp(-hdrColor * max(exposure, 0.1));
   
   // gamma correction 
  //  mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(resultWithAlpha);

    }}