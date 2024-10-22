#version 330 core
layout (location = 0) in vec3 aPos; // Vertex position

out vec3 TexCoords; // Pass to fragment shader

uniform mat4 view;
uniform mat4 projection;
uniform float time; // Time for rotation

void main()
{
    // Create a rotation angle based on time
    float angle = time * 0.07; // Control the speed of rotation

    // Create a rotation matrix (rotation around Y-axis)
    mat4 rotationMatrix = mat4(
        cos(angle), 0.0, sin(angle), 0.0,
        0.0, 1.0, 0.0, 0.0,
        -sin(angle), 0.0, cos(angle), 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    // Rotate the skybox position
    vec4 rotatedPosition = rotationMatrix * vec4(aPos, 1.0);
    
    // Pass through the transformed position to the vertex shader output
    TexCoords = aPos; // Skybox coordinates remain the same
    gl_Position = projection * view * rotatedPosition; // Apply view and projection transformations
}
