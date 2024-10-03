#extension GL_ARB_separate_shader_objects : enable

#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.1, 0.1, 0.7); // Default color (red)
}
