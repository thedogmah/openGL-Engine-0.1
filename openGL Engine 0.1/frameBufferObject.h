#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class FrameBufferObject {
private:
    //DOCS
    //This class was first created for the UI manager class, though a waterframebuffer
    //class also exists.
    GLuint fboID;
    GLuint textureID;
    GLuint depthBufferID;  // Added for depth buffer
public:
    FrameBufferObject(int width, int height, bool useDepthBuffer = false) {
        // Generate FBO
        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);

        // Generate texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

        // Generate and attach depth buffer if needed
        if (useDepthBuffer) {
            glGenRenderbuffers(1, &depthBufferID);
            glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);
        }

        // Unbind FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~FrameBufferObject() {
        glDeleteFramebuffers(1, &fboID);
        glDeleteTextures(1, &textureID);
    }

    GLuint getTextureID() const {
        return textureID;
    }

    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    }

    void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};