#ifndef GLTOOLS_H
#define GLTOOLS_H

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
#endif