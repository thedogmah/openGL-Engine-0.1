#include "Texture.h"

Texture::Texture(const std::string& filePath, GLenum textureType)
{
    this->type = textureType;
    this->filePath = filePath;
    glGenTextures(1, &textureID);
    glBindTexture(type, textureID);

    // Set texture wrapping and filtering options (adjust these as needed)
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load and generate the texture
    int width, height, numChannels;
    stbi_set_flip_vertically_on_load(true); // Flip the texture vertically (if needed)
    // Your code
    std::string modifiedFilePath = filePath;
    std::replace(modifiedFilePath.begin(), modifiedFilePath.end(), '\\', '/');

    // Use the modifiedFilePath variable in the stbi_load function
    unsigned char* data = stbi_load(modifiedFilePath.c_str(), &width, &height, &numChannels, 0);

    if (data) {
        GLenum format = (numChannels == 3) ? GL_RGB : GL_RGBA; // Adjust based on the image format

        glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(type);
    }
    else {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
    }

    stbi_image_free(data);
}

bool Texture::Load()
{
    glGenTextures(1, &textureID);
    glBindTexture(type, textureID);

    // Set texture wrapping and filtering options (adjust these as needed)
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load and generate the texture
    int width, height, numChannels;
    stbi_set_flip_vertically_on_load(true); // Flip the texture vertically (if needed)

    // Your code
    std::string modifiedFilePath = filePath;
    std::replace(modifiedFilePath.begin(), modifiedFilePath.end(), '\\', '/');

    // Use the modifiedFilePath variable in the stbi_load function
    unsigned char* data = stbi_load(modifiedFilePath.c_str(), &width, &height, &numChannels, 0);

    if (data) {
        GLenum format = (numChannels == 3) ? GL_RGB : GL_RGBA; // Adjust based on the image format

        glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(type);

        stbi_image_free(data);
        return true; // Texture loading was successful
    }
    else {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return false; // Texture loading failed
    }
}

void Texture::Bind(GLenum textureUnit) const
{
    //std::cout << "...ENTERING BIND FUNCTION";
    GLenum error = glGetError();
   /* if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error before activate texture unit: " << error << std::endl;
    }*/
    glActiveTexture(GL_TEXTURE0);//this must be a texture unit.
     error = glGetError();
    //if (error != GL_NO_ERROR) {
    //    std::cerr << "OpenGL error after activate texture unit: " << error << std::endl;
    //}
    glBindTexture(type, textureID);
 /*   if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error after bind texture unit: " << error << std::endl;
    }*/

    //std::cout << "...EXIT TEXTURE BIND FUNCTION";
}

void Texture::Unbind() const
{
    glBindTexture(type, 0);
}



