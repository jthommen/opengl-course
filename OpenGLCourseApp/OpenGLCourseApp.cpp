// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include<cmath>
#include<iostream>
#include<string>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


// Defining window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

// variables for pipeline parts
// VAO = Vertex Array Object
// VBO = Vertex Buffer Object
GLuint VAO, VBO, IBO, shader, uniformModel, uniformProjection;

// Control triangle movements
bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.008f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Shaders (normally in external file)
// Vertex Shader
// (takes individual points, allows manipulating them before
// passing them to the fragment shader)
// This shader scales the position and adds the W component
// (factor to transform to window coordinates)
// Important uniform variables:
// model: Where the 3D model is
// view: Where the camera is
// projection: How what the camera sees is translated to the screen (with depth)
static const char* vShader = "								\n"
"#version 330												\n"
"layout (location = 0) in vec3 pos;							\n"
"out vec4 vCol;												\n"
"uniform mat4 model;										\n"
"uniform mat4 projection;									\n"
"void main()												\n"
"{															\n"
"	gl_Position = projection * model * vec4(pos, 1.0);		\n"
"	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);				\n"
"}";

// Fragment Shader
// Takes the window-space XYZ position of the fragment
// Computes the output color(s)
static const char* fShader = "								\n"
"#version 330												\n"
"in vec4 vCol;												\n"
"out vec4 color;											\n"
"void main()												\n"
"{															\n"
"	color = vCol;											\n"
"}";

void createTriangle()
{
	// Telling openGL where the triangle is gonna be drawn
	// reusing indexed points to avoid duplicates
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	// Point of the triangles
	GLfloat vertices[] = {
	  -1.1f, -1.0f, 0.0f,
	  0.0f, -1.0f, 1.0f,
	  1.0f, -1.0f, 0.0f,
	  0.0f, 1.0f, 0.0f
	};

	// Creating VAO in the graphics card
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// create buffer for indices
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Create buffer object to put in VAO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Connect data (vertices) to the created buffer:
	// 1. Allocates memory on the GPU for vertex data
	// 2. Copies data from main memory to GPU memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Tells OpenGL what format our VBO data is.
	// Looks at array bound to GL_ARRAY_BUFFER. Params:
	// 1. Attribute index in the vertex shader
	// 2. Each vertex position has 3 values
	// 3. 32bit float values
	// 4. Not clear yet
	// 5. Spacing between the values (tightly packed 0)
	// 6. Byte offset (0 forstarting from beginning of VBO)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Unbinding buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

	// Will modify the code sitting in memory. Params:
	// 1. Shader object to put the string (code) into
	// 2. Number of strings to put into shader
	// 3. String (code) itself
	// 4. Length of string (code)
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

	// Get the ID of the uniform variabless
	uniformModel = glGetUniformLocation(shader, "model");
	uniformProjection = glGetUniformLocation(shader, "projection");

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

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Vertex positions are transformed into window coordinates
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Call functions to draw the triangle
	createTriangle();
	compileShaders();

	// Creating camera projection (adding depth)
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);

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

		if(direction)
		{
			curSize += 0.001f;
		}
		else {
			curSize -= 0.001f;
		}

		if (curSize >= maxSize || curSize <= minSize)
		{
			sizeDirection = !sizeDirection;
		}

		curAngle += 0.1f;
		if (curAngle >= 360) curAngle -= 360;

		// clear window color & depth buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// start shader program
		glUseProgram(shader);

		// Create 4x4 matrix (identity)
		glm::mat4 model = glm::mat4(1.0);

		// transform identity matrix with x translation
		model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f));
		model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

		// Assign values to shader, update each cycle
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		validateShader();

		// Rendering function. Draws one triangle for every 3 vertices. Params:
		// 1. Type of Data
		// 2. index in the glVertexAttribPointer specified data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// unassign shader program
		// (normally, many shaders would be called in cycles)
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}