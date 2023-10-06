#include "Texture.h"

Texture::Texture(const std::string& filePath, GLenum textureType)
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
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);
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

void Texture::Bind(GLenum textureUnit) const
{
   // glActiveTexture(textureUnit);
    glBindTexture(type, textureID);
}

void Texture::Unbind() const
{
    glBindTexture(type, 0);
}



