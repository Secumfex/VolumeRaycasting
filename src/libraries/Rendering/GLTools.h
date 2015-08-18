#ifndef GLTOOLS_H
#define GLTOOLS_H

#include "Core/DebugLog.h"
#include <Importing/Importer.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <glm/glm.hpp>


GLFWwindow* generateWindow(int width = 1280, int height = 720, int posX = 100, int posY = 100);
bool shouldClose(GLFWwindow* window);
void swapBuffers(GLFWwindow* window);
void destroyWindow(GLFWwindow* window);
void render(GLFWwindow* window, std::function<void (double)> loop);
GLenum checkGLError(bool printIfNoError = false);
std::string decodeGLError(GLenum error);

void setKeyCallback(GLFWwindow* window, std::function<void (int, int, int, int)> func);
void setMouseButtonCallback(GLFWwindow* window, std::function<void (int, int, int)> func);
void setCharCallback(GLFWwindow* window, std::function<void (unsigned int)> func);
void setCursorPosCallback(GLFWwindow* window, std::function<void (double, double)> func);
void setScrollCallback(GLFWwindow* window, std::function<void (double, double)> func);
void setCursorEnterCallback(GLFWwindow* window, std::function<void (int)> func);

glm::vec2 getResolution(GLFWwindow* window);
float getRatio(GLFWwindow* window);

template <typename T>
GLuint loadTo3DTexture(VolumeData<T>& volumeData, GLenum internalFormat = GL_R16I, GLenum format = GL_RED_INTEGER, GLenum type = GL_SHORT)
{
	GLuint volumeTexture;

	glEnable(GL_TEXTURE_3D);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &volumeTexture);
	glBindTexture(GL_TEXTURE_3D, volumeTexture);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

	// allocate GPU memory
	glTexStorage3D(GL_TEXTURE_3D
		, 1
		, GL_R16I
		, volumeData.size_x
		, volumeData.size_y
		, volumeData.size_z
	);

	// upload data
	glTexSubImage3D(GL_TEXTURE_3D
		, 0
		, 0
		, 0
		, 0
		, volumeData.size_x
		, volumeData.size_y
		, volumeData.size_z
		, GL_RED_INTEGER
		, GL_SHORT
		, &(volumeData.data[0])
	);

	return volumeTexture;
}

#endif