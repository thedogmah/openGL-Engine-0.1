#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 texCoords; // texture coords attribute
layout(location = 2) in vec3 aNormal;

struct InstanceData {
    vec3 objectColour;
	int ID;
	
	vec3 objectRandomColour;
	int nextID;
    mat4 modelMatrix;
    float rotationPerFrame;
};


layout (std430, binding = 2) buffer InstanceBuffer {
    InstanceData  instanceData[100];
};


uniform mat4 view;
uniform mat4 projection;

out vec2 textureCoords;
out vec3 Normal;
out vec3 FragPosWorld;
out vec3 mouseColour;
mat4 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    
    return mat4(
        c, 0.0, -s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        s, 0.0, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}
void main() {
mouseColour = instanceData[gl_InstanceID].objectColour;
  mat4 model = instanceData[gl_InstanceID].modelMatrix;
    float rotationAngle = radians(1.0); // Rotate by 1 degree per frame

    model = rotateY(rotationAngle) * model;
//vec4 worldPos = model*vec4(aPos, 1.0); //worldPos of every vertex

gl_Position = projection * view * model * vec4(aPos, 1.0);

 textureCoords = texCoords;
 Normal = mat3(transpose(inverse(view * model))) * aNormal;
 //Normal = mat3(model) * aNormal;
 FragPosWorld = vec3(model * vec4(aPos, 1.0));
   

}

