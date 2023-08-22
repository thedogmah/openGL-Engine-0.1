#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace DoMath{


	glm::vec3 calculateNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	std::vector<glm::vec3> generateNormalVector(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices);
	std::vector<glm::vec3> generateNormalVector(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
	std::vector<float> ripVertices(const std::vector<float>& vertices, int stride);

}