// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include<cmath>
#include<iostream>
#include<string>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>

// Defining window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

// variables for pipeline parts
// VAO = Vertex Array Object
// VBO = Vertex Buffer Object
GLuint VAO, VBO, shader, uniformXMove;

// Control triangle movements
bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

// Shaders (normally in external file)
// Vertex Shader
// (takes individual points, allows manipulating them before
// passing them to the fragment shader)
static const char* vShader = "                            \n"
"#version 330                                             \n"
"layout (location = 0) in vec3 pos;                       \n"
"uniform float xMove;									  \n"
"void main()                                              \n"
"{                                                        \n"
"  gl_Position = vec4(0.4*pos.x + xMove, 0.4*pos.y, pos.z, 1.0);  \n"
"}";

// Fragment Shader
static const char* fShader = "                            \n"
"#version 330                                             \n"
"out vec4 color;                                          \n"
"void main()                                              \n"
"{                                                        \n"
"  color = vec4(1.0, 0.0, 0.0, 1.0);                      \n"
"}";

void createTriangle()
{
	// Point of the triangles
	GLfloat vertices[] = {
	  -1.1f, -1.0f, 0.0f,
	  1.0f, -1.0f, 0.0f,
	  0.0f, 1.0f, 0.0f
	};

	// Creating VAO in the graphics card
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create buffer object to put in VAO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Connect data (vertices) to the created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Not clear what this does yet
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Unbinding buffer and vertex array
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

// Compiling & adding the shaders to the program
void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	// create empty shader
	GLuint theShader = glCreateShader(shaderType);

	// Where shader code is stored
	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	// Get shader code length
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	// Will modify the code sitting in memory
	glShaderSource(theShader, 1, theCode, codeLength);

	// Compile shader
	glCompileShader(theShader);

	// Validate compiled shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Check if compiling errors happened
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		// If not okay, get Info log and put it into the char array
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);

		// Then print error log
		std::cout << "Error compiling " << shaderType << " shader: " << eLog << std::endl;
		return;
	}

	glAttachShader(theProgram, theShader);
}

void compileShaders()
{
	shader = glCreateProgram();

	if (!shader) {
		std::cout << "Error creating shader program.\n";
		return;
	}

	addShader(shader, vShader, GL_VERTEX_SHADER);
	addShader(shader, fShader, GL_FRAGMENT_SHADER);

	// link and valiate the shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Create executables on the graphics card
	// Check if linking errors happened
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		// If not okay, get Info log and put it into the char array
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);

		// Then print error log
		std::cout << "Error linking program: " << eLog << std::endl;
		return;
	}

	// Get the ID of the xmove uniform variable
	uniformXMove = glGetUniformLocation(shader, "xMove");

}

void validateShader()
{
	// Check if validated program has errors
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		// If not okay, get Info log and put it into the char array
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);

		// Then print error log
		std::cout << "Error validating program: " << eLog << std::endl;
		return;
	}

}

int main()
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
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		std::cout << "GLFW window creation failed.\n";
		glfwTerminate();
		return 1;
	}

	// Set buffer size information
	// After window creation, need area in the window = buffer
	// Buffer holds all the openGL data that is being drawn to
	int bufferWidth, bufferHeight;
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

	// Set up viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Call functions to draw the triangle
	createTriangle();
	compileShaders();

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get & handle user input events
		glfwPollEvents();

		// while loop is running, check direction of triangle
		// steer accordingly
		if(direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		// If we reach max end points, change direction
		if (std::abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		// clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// start shader program
		glUseProgram(shader);

		// Assign value to shader, update each cycle
		glUniform1f(uniformXMove, triOffset);

		glBindVertexArray(VAO);
		validateShader();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// unassign shader program
		// (normally, many shaders would be called in cycles)
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}