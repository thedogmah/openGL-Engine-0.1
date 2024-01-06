#include "3D.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace DoMath {
    glm::vec3 calculateNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
    {
    
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        return glm::normalize(glm::cross(edge1, edge2));
    
    }

    //Generate normals from GLM vec3 and indices
    std::vector<glm::vec3> generateNormalVector(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices)
    {
        std::vector<glm::vec3> normals(vertices.size(), glm::vec3(0.0f));

        for (size_t i = 0; i < indices.size(); i += 3) {
            const glm::vec3& v1 = vertices[indices[i]];
            const glm::vec3& v2 = vertices[indices[i + 1]];
            const glm::vec3& v3 = vertices[indices[i + 2]];

            glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

            normals[indices[i]] += normal;
            normals[indices[i + 1]] += normal;
            normals[indices[i + 2]] += normal;
        }

        for (size_t i = 0; i < normals.size(); ++i) {
            normals[i] = glm::normalize(normals[i]);
        }

        return normals;
    
    }

    //Generate normals from float vector and indices
    std::vector<glm::vec3> generateNormalVector(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    {
         std::vector<glm::vec3> normals(vertices.size() / 3, glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i += 3) {
        const glm::vec3 v1(vertices[3 * indices[i]], vertices[3 * indices[i] + 1], vertices[3 * indices[i] + 2]);
        const glm::vec3 v2(vertices[3 * indices[i + 1]], vertices[3 * indices[i + 1] + 1], vertices[3 * indices[i + 1] + 2]);
        const glm::vec3 v3(vertices[3 * indices[i + 2]], vertices[3 * indices[i + 2] + 1], vertices[3 * indices[i + 2] + 2]);

        glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

        normals[indices[i]] += normal;
        normals[indices[i + 1]] += normal;
        normals[indices[i + 2]] += normal;
    }

    for (size_t i = 0; i < normals.size(); ++i) {
        normals[i] = glm::normalize(normals[i]);
    }

    return normals;
    }


    //function takes a vector of mesh data (vertices, uv colour etc) and returns a vector of position data only
    //function assumes position is at the start
    std::vector<float> ripVertices(const std::vector<float>& vertices, int stride) 
    {
    
        std::vector<float> positions;
        int i = 0;
        for (auto data : vertices) {
            if (i < 3) {
                positions.push_back(data);
            }
        
        
            i++;
            if (i == stride )
            {
                i = 0;
            }
            
        }

        return positions;

    }


}

