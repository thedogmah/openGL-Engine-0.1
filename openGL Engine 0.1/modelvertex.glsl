#version 450 core


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 weights;

// Uniforms for transformation matrices
uniform mat4 model;                          // Model matrix
uniform mat4 view;                           // View matrix
uniform mat4 projection;                     // Projection matrix

out vec2 TexCoords;                          // Pass to fragment shader
out vec3 FragNormal;                         // Pass to fragment shader
out vec3 FragPos;                            // Fragment position in world space

void main() {
    // Compute vertex position in clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Pass texture coordinates to fragment shader
    TexCoords = aTexCoord;
    TexCoords.y = 1.0 - TexCoords.y; // Flip Y for correct texture mapping

    // Transform vertex normal to world space
    FragNormal = mat3(transpose(inverse(model))) * aNormal;

    // Compute fragment position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));

}
