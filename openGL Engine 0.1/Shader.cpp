#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {

	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);


	try {
	
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}

	catch(std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;

		}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//compile shaders

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	//print compile errors if.
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	//fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
	//shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);//attach each shader individually
	glAttachShader(ID, fragment);
	glLinkProgram(ID);//link the whole program when complete  so it can be executed by CPU
	
//print any link errors
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//delete the shaders since they are linked to the program and not necessary.
	glDeleteShader(vertex);
	glDeleteShader(fragment); std::cout << "Paths: " << fragmentPath << ", " << vertexPath;
	std::cout << "\nShader constructor, program ID is:" << ID;
	
}

void Shader::Shader::use() {
	
	//activate the ID assigned;
	glUseProgram(ID);
	std::cout << "Shader ID:" << ID;
}
template <typename T>
void Shader::setUniform(const std::string name, const T& value) const
{

	static_assert(std::is_same<T, bool>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, float>::value || 
		std::is_same<T, glm::vec2>::value ||
		std::is_same<T, glm::vec3>::value ||
		std::is_same<T, glm::vec4>::value,
		"Unsupported uniform type");

	if constexpr (std::is_same<T, bool>::value) {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
	}
	else if constexpr (std::is_same<T, int>::value)
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	else if constexpr (std::is_same<T, float>::value)
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

 else if constexpr (std::is_same<T, glm::vec2>::value) {
	 glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
	}
 else if constexpr (std::is_same<T, glm::vec3>::value) {
	 glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
	}
 else if constexpr (std::is_same<T, glm::vec4>::value) {
	 glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
	}

}

// Function for matrix uniforms
void Shader::setMatrixUniform(const GLint& shaderid, const glm::mat4& matrix) const
{
	
	 GLuint location = glGetUniformLocation(shaderid,"view");
	glUniformMatrix4fv(location,  1, GL_FALSE, glm::value_ptr(matrix));
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL error: " << error << std::endl;

	}
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::bindAttributes() {
	// Implementation of bindAttributes
	// Example: glBindAttribLocation(ID, 0, "position");
}

void Shader::getAllUniformLocations() {
	// Implementation of getAllUniformLocations
	// Example: location_modelMatrix = glGetUniformLocation(ID, "modelMatrix");
}