// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include<cmath>
#include<iostream>
#include<string>
#include<vector>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "GLwindow.h"
#include "Camera.h"

const float toRadians = 3.14159265f / 180.0f;

GLwindow mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

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
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
// Takes the window-space XYZ position of the fragment
// Computes the output color(s)
static const char* fShader = "Shaders/shader.frag";

void CreateObjects()
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

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj2);

}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	// Create Window
	mainWindow = GLwindow(800, 600);
	mainWindow.Initialize();

	// Call functions to draw the triangle
	CreateObjects();
	CreateShaders();

	// initialize camera
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.2f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;

	// Creating camera projection (adding depth)
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		// Calculating delta time
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get & handle user input events
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// clear window color & depth buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// start shader program
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();

		// Create 4x4 matrix (identity)
		glm::mat4 model = glm::mat4(1.0);

		// transform identity matrix with x translation
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		// Assign values to shader, update each cycle
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		meshList[0]->RenderMesh();

		// Working with second object (normally go into object class)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();


		// unassign shader program
		// (normally, many shaders would be called in cycles)
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}