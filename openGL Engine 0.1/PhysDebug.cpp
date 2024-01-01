
#define BT_USE_DOUBLE_PRECISION
#include "PhysDebug.h"


PhysDebug::PhysDebug()
	{
				//create shaders from member variables 
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
		std::stringstream vertexStringStream;
		vertexStringStream << vertexShaderSource;
		const std::string vertexString = vertexStringStream.str();
		//new string copied over into a pointer to char to pass to the function.
		const char* vertexSourcePtr = vertexString.c_str();

		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at shader program PHys Debug class11 " << error << std::endl;



		}
		unsigned int basicVertex;
		basicVertex = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(basicVertex, 1, &vertexShaderSource, 0);
		glCompileShader(basicVertex);

		int  success;
		char infoLog[512];
		glGetShaderiv(basicVertex, GL_COMPILE_STATUS, &success);
		
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at shader program PHys Debug class22 " << error << std::endl;



		}
		if (!success) {
			glGetShaderInfoLog(basicVertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		else {

			std::cout << "Vertex shader compiled successfully";

		}
		
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at shader program PHys Debug class33 " << error << std::endl;



		}
		std::stringstream fragStringStream;
		fragStringStream << fragmentShaderSource;
		const std::string fragString = fragStringStream.str();

		const char* fragSourcePtr = fragString.c_str();

		unsigned int basicFragment;
		basicFragment = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(basicFragment, 1, &fragSourcePtr, 0);
		glCompileShader(basicFragment);

		glGetShaderiv(basicFragment, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(basicFragment, 512, NULL, infoLog);
			std::cout << "\nERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		else {

			std::cout << "\nFragment shader compiled successfully";

		}

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, basicVertex);
		glAttachShader(shaderProgram, basicFragment);
		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);
		//need to activate and deactivate shader program, and also

	
		while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error at shader program PHys Debug class end " << error << std::endl;



		}
		shaderPtr = &shaderProgram;
		 viewLocation = glGetUniformLocation(shaderProgram, "view");
		 projectionLocation = glGetUniformLocation(shaderProgram, "projection");
	}

	void PhysDebug::SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix)
	{
		
		glUseProgram(shaderProgram);
	//	std::cout << "\nDebugger shader program name: " << shaderProgram;
	
	
	//DEBUG commented out to optomise. Variables moved to members and set in constructor.
		/*	while ((error = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error 1 at setMatrices program Phys Debug class " << error << std::endl;



		}*/
	/*	int viewLocation = glGetUniformLocation(shaderProgram, "view");
		int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
	*/ 
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(pViewMatrix));
		glUniformMatrix4fv(projectionLocation,1, GL_FALSE, glm::value_ptr(pProjectionMatrix));
		if (boolShowGLErrors) {
			GLenum error;
			while ((error = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL Error 1 at setMatrices end program Phys Debug class " << error << std::endl;



			}
		}
	}

	void PhysDebug::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{

		//  shaderProgram.setVec3("lineColor", glm::vec3(color.x(), color.y(), color.z()));
		glUseProgram(shaderProgram);
		// Specify the vertices of the line directly in the shader
		GLfloat points[12];
		
		btVector3 playerPosition = character->getRigidBody()->getWorldTransform().getOrigin();

		// Calculate the distance between 'from' and 'to' points and the player's position
		btScalar distanceFrom = (from - playerPosition).length();
		btScalar distanceTo = (to - playerPosition).length();

		// Specify your distance threshold here (e.g., 10 meters)
		btScalar distanceThreshold = 10.0;

		// Check if either 'from' or 'to' points are within the distance threshold
		if (distanceFrom <= distanceThreshold || distanceTo <= distanceThreshold) {
			// Draw the line only if it's within the specified distance
			// ... Your existing drawing code here ...


			points[0] = from.x();
			points[1] = from.y();
			points[2] = from.z();
			points[3] = color.x();
			points[4] = color.y();
			points[5] = color.z();

			points[6] = to.x();
			points[7] = to.y();
			points[8] = to.z();
			points[9] = 1.0;
			points[10] = color.y();
			points[11] = color.z();


			//		std::cout << from.x() << ", " << from.y() << ", " << from.z() <<"\n";


			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
			//	glBindVertexArray(0);

			//	glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, 2);
			glBindVertexArray(0);
		}
		//setDrawLinePositions.insert(from);
	//	setDrawLinePositions.insert(to);
	//DEBUG LINE	std::cout << "\n\nMake sure you're setting the shader program back after claling this function\n(the Drawline function in the debug drawern\notherwise the wrong shader will be selected";
	}

	void PhysDebug::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
	{
	}

