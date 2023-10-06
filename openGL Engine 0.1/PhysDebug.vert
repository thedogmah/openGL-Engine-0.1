        const char* vertexShaderSource = R"(
 #version 330 core

precision mediump float;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec2 uvs;
out vec3 fColor; // Output color
out vec3 Normal;  // Output normal in world space
out vec4 fragPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int applyModelTransform;

out vec2 uvsOut;
void main()
{

 Normal = normals; // Pass normals to the fragment shader
Normal = normalize(normals);
uvsOut = uvs;
   // fragPosition = projection * vec4(position,1.0f); 

 if (applyModelTransform == 1) {
        gl_Position = projection * view * model * vec4(position, 1.0f);
   fColor = color; // Pass color directly to the fragment shader
 fragPosition = projection *  vec4(position, 1.0f);
}
else{
    gl_Position = projection * view * vec4(position, 1.0f);
    fColor = color; // Pass color directly to the fragment shader
}
}