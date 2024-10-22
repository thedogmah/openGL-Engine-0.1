// Vertex Shader
#version 430

layout (location = 0) in vec3 aPos; // Vertex position attribute

uniform mat4 view; // View matrix 
uniform mat4 projection; // Projection matrix
out vec3 pos;
void main()
{
pos = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0); 
}