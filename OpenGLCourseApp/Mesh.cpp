#include "pch.h"
#include "Mesh.h"


Mesh::Mesh(): VAO{0}, VBO{0}, IBO{0}, indexCount{0}
{
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

	// Creating VAO in the graphics card
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// create buffer for indices
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	// Create buffer object to put in VAO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Connect data (vertices) to the created buffer:
	// 1. Allocates memory on the GPU for vertex data
	// 2. Copies data from main memory to GPU memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

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

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);

	// Rendering function. Draws one triangle for every 3 vertices. Params:
	// 1. Type of Data
	// 2. index in the glVertexAttribPointer specified data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}

Mesh::~Mesh()
{
	ClearMesh();
}
