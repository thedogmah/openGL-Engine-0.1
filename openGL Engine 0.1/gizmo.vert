#version 420 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

// Uniforms
uniform mat4 model;                         // Model matrix
uniform mat4 view;                          // View matrix
uniform mat4 projection;                    // Projection matrix


out vec3 fragColor;

void main() {
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
    fragColor = inColor;
}
