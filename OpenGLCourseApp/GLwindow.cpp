#include "pch.h"
#include "GLwindow.h"


GLwindow::GLwindow(): width{800}, height{600}
{ for(size_t i = 0; i < 1024; i++) keys[i] = 0; }

GLwindow::GLwindow(GLint windowWidth, GLint windowHeight): width{windowWidth}, height{windowHeight}
{ for (size_t i = 0; i < 1024; i++) keys[i] = 0; }

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

	// Handle key + mouse input
	createCallbacks();
	// Lock mouse to the screen
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	// Needed to pass right window object to static callback function in window class
	glfwSetWindowUserPointer(mainWindow, this);

	return 0;
}

void GLwindow::createCallbacks()
{
	// When key pressed on main window, pass on to handle keys
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat GLwindow::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat GLwindow::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void GLwindow::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	// Returns pointer to our window
	GLwindow* theWindow = static_cast<GLwindow*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// check for valid key
	if (key >= 0 && key < 1024)
	{
		// Toggle key press map
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
		}
	}
}

void GLwindow::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	GLwindow* theWindow = static_cast<GLwindow*>(glfwGetWindowUserPointer(window));
	
	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	// Prevent inverted up and down movements
	theWindow->yChange = theWindow->lastY - yPos;

	// Update for next iteration
	theWindow->lastX = xPos;
	theWindow->lastY = yPos; 

}



GLwindow::~GLwindow()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
