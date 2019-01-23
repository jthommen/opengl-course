#pragma once

#include<cstdio>
#include<iostream>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

class GLwindow
{
public:
	GLwindow();
	GLwindow(GLint windowWidth, GLint windowHeight);

	int Initialize();

	GLint getBufferWidth() { return bufferHeight; }
	GLint getBufferHeight() { return bufferHeight; }

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	void swapBuffers() { glfwSwapBuffers(mainWindow); }


	~GLwindow();

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	GLint bufferWidth, bufferHeight;
};

