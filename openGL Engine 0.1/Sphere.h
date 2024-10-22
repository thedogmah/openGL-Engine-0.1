#include <vector>
#include <glm/glm.hpp>
#include "glad/glad.h" // or your OpenGL loader of choice
#include "GLFW/glfw3.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif
class Sphere {
public:
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;

    Sphere(float radius, unsigned int rings, unsigned int sectors) {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<GLuint> indices;

        float const R = 1.0f / (float)(rings - 1);
        float const S = 1.0f / (float)(sectors - 1);

        for (unsigned int r = 0; r < rings; r++) {
            for (unsigned int s = 0; s < sectors; s++) {
                float const y = sin(-M_PI_2 + M_PI * r * R);
                float const theta = 2 * M_PI * s * S; // Full circle for theta
                float const x = cos(theta) * sin(M_PI * r * R);
                float const z = sin(theta) * sin(M_PI * r * R);

                // Normalize and scale to radius
                glm::vec3 point(x, y, z);
                vertices.push_back(glm::normalize(point) * radius);

                // Texture coordinates for debugging
                texCoords.push_back(glm::vec2(s * S, r * R));
            }
        }

        for (unsigned int r = 0; r < rings - 1; r++) {
            for (unsigned int s = 0; s < sectors - 1; s++) {
                indices.push_back(r * sectors + s);
                indices.push_back((r + 1) * sectors + s);
                indices.push_back((r + 1) * sectors + (s + 1));

                indices.push_back(r * sectors
                    + s);
                indices.push_back((r + 1) * sectors + (s + 1));
                indices.push_back(r * sectors + (s + 1));
            }

            // Connect the last sector to the first to complete the ring
            indices.push_back(r * sectors + (sectors - 1));
            indices.push_back((r + 1) * sectors + (sectors - 1));
            indices.push_back((r + 1) * sectors);

            indices.push_back(r * sectors + (sectors - 1));
            indices.push_back((r + 1) * sectors);
            indices.push_back(r * sectors);
        }

        indexCount = indices.size();

        // OpenGL buffer setup remains largely the same
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(glm::vec3) + sizeof(glm::vec2)), nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
        glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), texCoords.size() * sizeof(glm::vec2), texCoords.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        // Vertex position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(vertices.size() * sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0); // Unbind VAO
    }
    void draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // Unbind VAO
    }

    ~Sphere() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};
