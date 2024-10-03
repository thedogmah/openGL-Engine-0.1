#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoords; // Passing to fragment shader
out vec3 FragNormal; // Passing normal to fragment shader
out vec3 FragPos;    // Position in world space

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Pass texture coordinates to fragment shader
    TexCoords = aTexCoord; 
    TexCoords.y = 1.0 - TexCoords.y; // Flip Y coordinate for texture mapping

    // Pass the fragment position and normal
    FragPos = vec3(model * vec4(aPos, 1.0)); // Convert to world space
    FragNormal = normalize(mat3(transpose(inverse(model))) * aNormal); // Normal transformation
}
