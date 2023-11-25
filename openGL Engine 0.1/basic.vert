#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 texCoords; // texture coords attribute
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aColor;

struct InstanceData {
    //from parent class worldObject
    vec3 objectColour;
    int ID;
    vec3 objectRandomColour;
    int nextID;

    //from child class 'cubeInstance'
    mat4 modelMatrix;
    float rotationPerFrame;
     uvec2 padding;

    // Pointer to rigid body
    uint rigidBodyPtr;
};

layout (std430, binding = 2) buffer InstanceBuffer {
    InstanceData  instanceData[100];
};

uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelUniform;
uniform int isInstanced;
uniform float drawDistance;
out vec2 textureCoords;
out vec3 Normal;
out vec3 FragPosWorld;
out vec3 mouseColour;
out vec3 FragColor;

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

vec3 cameraPosition = vec3(inverse(view)[3]);
float cameraX = cameraPosition.x;
float cameraZ = cameraPosition.z;
//DIFFERNET LOD METHODS AND BILLBOARDS.
if (isInstanced != 0) {
    // Calculate distance from the camera's x, z position
    float distanceToCamera = distance(vec2(cameraX, cameraZ), vec2(instanceData[gl_InstanceID].modelMatrix[3].x, instanceData[gl_InstanceID].modelMatrix[3].z));

    // Move the vertex to a position that effectively discards it
    if (distanceToCamera > drawDistance) {
        gl_Position = vec4(0.0, 0.0, 2.0, 1.0);  // Move to the center of clip space
        return;
    }
    else{
        FragColor = aColor;
        mouseColour = instanceData[gl_InstanceID].objectColour;
        mat4 model = instanceData[gl_InstanceID].modelMatrix;

        // Apply the model matrix to the vertex position
        gl_Position = projection * view * model * vec4(aPos, 1.0);

        textureCoords = texCoords;
        Normal = mat3(transpose(inverse(view * model))) * aNormal;
        FragPosWorld = vec3(model * vec4(aPos, 1.0));
        }
    }
    else {
        // Non-instanced data handling here
        FragColor = aColor; // Use the color data from layout 3
        mouseColour = vec3(0.0); // Set mouseColour to a default value
        // You can also set other variables to defaults here

        // Use the model matrix from a uniform variable
        mat4 model = modelUniform; // Replace with your uniform variable name

        // Apply transformations (if needed) to the model matrix
        // For example:
        // model = rotateY(rotationAngle) * model;
        // model = translate(model, vec3(x, y, z));

        // Apply the model matrix to the vertex position
        gl_Position = projection * view * model * vec4(aPos, 1.0);

        textureCoords = texCoords;
        Normal = mat3(transpose(inverse(view * model))) * aNormal;
        FragPosWorld = vec3(model * vec4(aPos, 1.0));
    }
}