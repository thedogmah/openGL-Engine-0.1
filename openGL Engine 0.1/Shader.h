#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ObjImporter.h"

#include "globals.h"

struct Material {
    Material() {};
    glm::vec3 ambient;       // Ka MTL format KA for ambient (the 'a')
    glm::vec3 diffuseColor;  // Kd 
    glm::vec3 specularColor; //Ks
    glm::vec3 emissiveColor; //Ke

    float shininess;        //Ns (specular component)
    float transparency;     //d (transparency
    float indexOfRefraction;//Ni index of refraction
    int illuminationModel;  //illuination
   
    Material(const Material& other)
        : ambient(other.ambient),
        diffuseColor(other.diffuseColor),
        specularColor(other.specularColor),
        emissiveColor(other.emissiveColor),
        shininess(other.shininess),
        transparency(other.transparency),
        indexOfRefraction(other.indexOfRefraction),
        illuminationModel(other.illuminationModel) {
        // Deep copy the texture if necessary
         
         }
};
class Shader
{

public:

    // program ID
    unsigned int ID;


    Shader(const char* vertexPath, const char* fragmentPath);

    void setMatrixUniform(const GLint& name, const glm::mat4& matrix) const;
    void unbind();


    // New function to set the entire Material struct
    void setMaterial(const Material& material) const {
        setUniform("material.ambient", material.ambient);
        setUniform("material.diffuseColor", material.diffuseColor);
        setUniform("material.specularColor", material.specularColor);
        setUniform("material.emissiveColor", material.emissiveColor);
        setUniform("material.shininess", material.shininess);
        setUniform("material.transparency", material.transparency);
        setUniform("material.indexOfRefraction", material.indexOfRefraction);
        setUniform("material.illuminationModel", material.illuminationModel);
    }
    // Move the template definition here
    template <typename T>

    void setUniform(const std::string& name, const T& value) const {
        GLuint location = glGetUniformLocation(ID, name.c_str());

        //static_assert(std::is_same<T, bool>::value ||
        //    std::is_same<T, int>::value ||
        //    std::is_same<T, float>::value ||
        //    std::is_same<T, glm::vec2>::value ||
        //    std::is_same<T, glm::vec3>::value ||
        //    std::is_same<T, glm::vec4>::value ||
        //    std::is_same<T, Material>::value, // Include Material in the check
        //    "Unsupported uniform type");

        if constexpr (std::is_same<T, bool>::value) {
            glUniform1i(location, static_cast<int>(value));
        }
        else if constexpr (std::is_same<T, int>::value) {
            glUniform1i(location, value);
        }
        else if constexpr (std::is_same<T, float>::value) {
            glUniform1f(location, value);
        }
        else if constexpr (std::is_same<T, glm::vec2>::value) {
            glUniform2fv(location, 1, glm::value_ptr(value));
        }
        else if constexpr (std::is_same<T, glm::vec3>::value) {
            glUniform3fv(location, 1, glm::value_ptr(value));
        }
        else if constexpr (std::is_same<T, glm::vec4>::value) {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
        else if constexpr (std::is_same<T, Material>::value) {  // Add case for Material
            // Set each property of the Material struct
            glUniform3fv(glGetUniformLocation(ID, (name + ".ambient").c_str()), 1, glm::value_ptr(value.ambient));
            glUniform3fv(glGetUniformLocation(ID, (name + ".diffuseColor").c_str()), 1, glm::value_ptr(value.diffuseColor));
            glUniform3fv(glGetUniformLocation(ID, (name + ".specularColor").c_str()), 1, glm::value_ptr(value.specularColor));
            glUniform1f(glGetUniformLocation(ID, (name + ".shininess").c_str()), value.shininess);
            glUniform1f(glGetUniformLocation(ID, (name + ".transparency").c_str()), value.transparency);
            glUniform1f(glGetUniformLocation(ID, (name + ".indexOfRefraction").c_str()), value.indexOfRefraction);
            glUniform1i(glGetUniformLocation(ID, (name + ".illuminationModel").c_str()), value.illuminationModel);
        }
        // Add this case for glm::mat4
        else if constexpr (std::is_same<T, glm::mat4>::value) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

	
	void use();
	void setProgramID(unsigned int ID);// ID is set in the constructor, returned from gl function
	void checkCompileErrors(GLuint shader, std::string type);
	
	virtual void bindAttributes();
	virtual void getAllUniformLocations();

};

