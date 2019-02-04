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

	bool* getsKeys() { return keys;  }
	GLfloat getXChange();
	GLfloat getYChange();
		
	void swapBuffers() { glfwSwapBuffers(mainWindow); }


	~GLwindow();

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	GLint bufferWidth, bufferHeight;

	// mapping for key presses
	bool keys[1024];

	// handle mouse movements
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;

	// prevent jumps when mouse movement is initialized
	bool mouseFirstMoved;

	void createCallbacks();
	// call back when key is pressed
	// callback can't go to member function of a class -> static
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

