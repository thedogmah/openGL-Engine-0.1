#include "openGLDrawable.h"
#include <vector>
struct Vertex;
GLenum error;
openGLDrawable::openGLDrawable(std::vector<Vertex>& vertices) : vertices(vertices)
{//struct Vertex {
//    float x, y, z;       // Vertex coordinates
//    float u, v;          // Texture coordinates
//    float nx, ny, nz;    // Normal coordinates
//    float r, g, b, a;
//    // Color (red, green, blue, alpha)
//};
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error start of openGLDrawable constructor: " << error << std::endl;
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after binding VAO in openGLDrawable constructor: " << error << std::endl;
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after binding VBO in openGLDrawable constructor: " << error << std::endl;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error after buffer data of openGLDrawable constructor: " << error << std::endl;
	}	//size, data, useage

	//vertex coord
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
		// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, u)));

		// Normal coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx)));
	
		// Color attributes
	glEnableVertexAttribArray(3);	
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));

	glBindVertexArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error end of openGLDrawable constructor: " << error << std::endl;
	}
}

void openGLDrawable::draw()
{
	
	//
	
		glBindVertexArray(VAO); // Bind the VAO
	//	std::cout << vertices.size();
		glDrawArrays(GL_TRIANGLES, 0, 36); // Draw the object
		glBindVertexArray(0); // Unbind the VAO
	

}


//struct Vertex {
//	float x, y, z;       // Vertex coordinates
//	float u, v;          // Texture coordinates
//	float nx, ny, nz;    // Normal coordinates
//	float r, g, b, a;
//	// Color (red, green, blue, alpha)
//};