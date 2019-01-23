#include "pch.h"
#include "GLwindow.h"


GLwindow::GLwindow(): width{800}, height{600} { }
GLwindow::GLwindow(GLint windowWidth, GLint windowHeight): width{windowWidth}, height{windowHeight} { }

int GLwindow::Initialize()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cout << "Initialization failed.\n";
		glfwTerminate();
		return 1; // error
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Core profile means no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibiliy
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create glfw window
	// Params: width, heigh, title, multi monitors, shared across systems
	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		std::cout << "GLFW window creation failed.\n";
		glfwTerminate();
		return 1;
	}

	// Set buffer size information
	// After window creation, need area in the window = buffer
	// Buffer holds all the openGL data that is being drawn to
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use (could switch between windows)
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features (enable glew experimental)
	glewExperimental = GL_TRUE;

	// Initialize glew
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialization failed.\n";
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Vertex positions are transformed into window coordinates
	glViewport(0, 0, bufferWidth, bufferHeight);
	return 0;
}

GLwindow::~GLwindow()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
