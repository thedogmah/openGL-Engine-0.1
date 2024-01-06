#pragma once
#include <iostream>
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class waterFrameBuffers {
	//window_height = 800, window_width = 1400;
	  int REFLECTION_WIDTH = window_width;
	  int REFLECTION_HEIGHT = window_height;

	  int REFRACTION_WIDTH = window_width;
	  int REFRACTION_HEIGHT = window_height;

	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;

	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

	
public:

	waterFrameBuffers() {
	
		initialiseReflectionFrameBuffer();
		initialiseRefractionFrameBuffer();
	}
	GLuint reflectionTextureID;
	GLuint refractionTextureID; //IDs to accommodate renderDoc debugging

	GLuint getReflectionBufferID() {
		return reflectionFrameBuffer;
	}

	GLuint getRfractionBufferID() {
		return refractionFrameBuffer;
	}

	// Getter for reflectionFrameBuffer
	GLuint getRefractionTexture()  {
		return refractionTexture;
	}

	// Getter for reflectionTexture
	GLuint getReflectionTexture()  {
		return reflectionTexture;
	}

	// Getter for reflectionDepthBuffer
	GLuint getRefractionDepthTexture()  {
		return refractionDepthTexture;
	}


	void bindReflectionFrameBuffer()  {
		//glActiveTexture(GL_TEXTURE0);
	//	glActiveTexture(GL_TEXTURE14);
	//	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
		bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
		std::cout << "\nReflection fbo ID: " << reflectionFrameBuffer;
	//	glBindTexture(GL_TEXTURE_2D, 0);
	}
		
	void bindRefractionFrameBuffer()  {
		bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
		std::cout << "Refraction fbo ID: " << refractionFrameBuffer;
	}


public:

	void initialiseReflectionFrameBuffer() {
	
		int width = window_width;
		int height = window_height;
	

		glGenFramebuffers(1, &reflectionFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);

		// Color Attachment
		glGenTextures(1, &reflectionTexture);
		glBindTexture(GL_TEXTURE_2D, reflectionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);

		// Depth Attachment (using a depth buffer attachment)
		glGenRenderbuffers(1, &reflectionDepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionDepthBuffer);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "\nFramebuffer is not complete: " << status << std::endl;
		}
		else {
			std::cout << "\nfbo texture attachment buffer complete";
		}
		//reflectionFrameBuffer = frameBuffer;
		// Unbind the frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void initialiseRefractionFrameBuffer() {

		int width = window_width;
		int height = window_height;
		glGenFramebuffers(1, &refractionFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);

		// Color Attachment
		glGenTextures(1, &refractionTexture);
		glBindTexture(GL_TEXTURE_2D, refractionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);

		// Depth Attachment (using a depth texture attachment)
		glGenTextures(1, &refractionDepthTexture);
		glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthTexture, 0);

		// Check framebuffer completeness
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Refraction Framebuffer is not complete: " << status << std::endl;
		}

		// Unbind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void bindFrameBuffer(GLuint frameBuffer, int width, int height) {
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		//glActiveTexture(GL_TEXTURE14);
		
		if (frameBuffer == reflectionFrameBuffer)
		{
			glActiveTexture(GL_TEXTURE14);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, reflectionTexture);
			std::cout << "\nReflection texture bound";
		}
		else
		{
			glActiveTexture(GL_TEXTURE15);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, refractionTexture);
			std::cout << "\nRefraction texture bound";
		}
	}

	void unbindCurrentFrameBuffer(){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		int screenWidth, screenHeight;
		glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);
	}

	GLuint createFrameBuffer() {
	
		GLuint frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);//Telling opengl which colorbuffer attchment 
		//in the currently bound fbo we want to render to
		checkGLError("Create Frame Buffer");
		std::cout << "\nFrame buffer ID: " << frameBuffer << std::endl;
		return frameBuffer;
	}


	GLuint createTextureAttachment(int width, int height) {


		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Attach the texture to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		// Check framebuffer completeness
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "\nFramebuffer is not complete: " << status << std::endl;
		}
		else {
			std::cout << "\nfbo texture attachment buffer complete";
		}

		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

	GLuint createDepthTextureAttachment(int width, int height){
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

		// Check framebuffer completeness
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "\nFramebuffer is not complete: " << status << std::endl;
		}
		else {
			std::cout << "\nFramebuffer DepthTexture attachment complete";
		}

		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

	GLuint createDepthBufferAttachment(int width, int height) {
		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

		// Check framebuffer completeness
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "\nFramebuffer is not complete: " << status << std::endl;
		}
		else {
			std::cout << "\nFramebuffer depth buffer attachment complete";
		}

		// Unbind the renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		return depthBuffer;
	}

	void cleanUp(){
		glDeleteFramebuffers(1, &reflectionFrameBuffer);
		glDeleteTextures(1, &reflectionTexture);
		glDeleteRenderbuffers(1, &reflectionDepthBuffer);
		glDeleteFramebuffers(1, &refractionFrameBuffer);
		glDeleteTextures(1, &refractionTexture);
		glDeleteTextures(1, &refractionDepthTexture);
	}
};