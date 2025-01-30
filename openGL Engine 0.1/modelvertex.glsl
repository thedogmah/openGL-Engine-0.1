#version 330 core

// Vertex attributes
layout(location = 0) in vec3 position;       // Vertex position
layout(location = 1) in vec3 normal;         // Vertex normal
layout(location = 2) in vec2 texCoords;      // Texture coordinates
layout(location = 3) in ivec4 boneIDs;       // Bone IDs influencing this vertex
layout(location = 4) in vec4 boneWeights;    // Weights of the bones for this vertex

// Uniforms
uniform mat4 model;                         // Model matrix
uniform mat4 view;                          // View matrix
uniform mat4 projection;                    // Projection matrix
uniform mat4 boneMatrices[100];             // Array of bone matrices (adjust 100 to MAX_BONES as needed)
uniform int isAnimation;                    // Toggle for animation

// Outputs to fragment shader
out vec2 TexCoords;                         // Pass texture coordinates to fragment shader
out vec3 FragPos;                           // Pass fragment position for lighting
out vec3 Normal;                            // Pass normal for lighting

void main()
{
    vec4 worldPosition;
    mat3 normalMatrix;

    if (isAnimation == 1) {
    vec4 normalizedWeights = boneWeights / max(0.0001, dot(boneWeights, vec4(1.0)));
mat4 boneTransform = normalizedWeights.x * boneMatrices[boneIDs.x] +
                     normalizedWeights.y * boneMatrices[boneIDs.y] +
                     normalizedWeights.z * boneMatrices[boneIDs.z] +
                     normalizedWeights.w * boneMatrices[boneIDs.w];

        // Apply model matrix and bone transformation
        worldPosition = model * boneTransform * vec4(position, 1.0);

        // Transform normal to world space and account for bone transformations
        normalMatrix = mat3(transpose(inverse(mat3(model * boneTransform))));

    } 
    
    else {
        // No bone transformation, only apply the model matrix
        worldPosition = model * vec4(position, 1.0);

        // Transform normal to world space using only the model matrix
        normalMatrix = mat3(transpose(inverse(model)));
    }

    // Output position to the next stage (the fragment shader)
    gl_Position = projection * view * worldPosition;

    // Pass data to the fragment shader
    TexCoords = vec2(texCoords.x, 1.0 - texCoords.y);
    FragPos = vec3(worldPosition);

    // Pass the normalized normal
    Normal = normalize(normalMatrix * normal);
}
