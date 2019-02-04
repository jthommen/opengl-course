#include "pch.h"
#include "Shader.h"


Shader::Shader(): shaderID{0}, uniformModel{0}, uniformProjection{0} { }

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open())
	{
		std::cout << "Failed to read " << fileLocation << "!. File doesn't exist.\n";
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	shaderID = glCreateProgram();

	if (!shaderID) {
		std::cout << "Error creating shader program.\n";
		return;
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	// link and valiate the shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Create executables on the graphics card
	// Check if linking errors happened
	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		// If not okay, get Info log and put it into the char array
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);

		// Then print error log
		std::cout << "Error linking program: " << eLog << std::endl;
		return;
	}

	// Get the ID of the uniform variabless
	uniformModel = glGetUniformLocation(shaderID, "model");
	uniformProjection = glGetUniformLocation(shaderID, "projection");
	uniformView = glGetUniformLocation(shaderID, "view");

	ValidateShader();
}

GLuint Shader::GetProjectionLocation()
{
	return uniformProjection;
}

GLuint Shader::GetModelLocation()
{
	return uniformModel;
}

GLuint Shader::GetViewLocation()
{
	return uniformView;
}

void Shader::UseShader()
{
	glUseProgram(shaderID);
}

void Shader::ClearShader()
{
	if (shaderID != 0)
	{
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
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

void Shader::ValidateShader()
{
	// Check if validated program has errors
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		// If not okay, get Info log and put it into the char array
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);

		// Then print error log
		std::cout << "Error validating program: " << eLog << std::endl;
		return;
	}
}

Shader::~Shader()
{
	ClearShader();
}
