#include <iostream>
#include <fstream>
#include <sstream>

#include "shaderprogram.h"
#include "contract.h"

ShaderProgram::ShaderProgram()
{
	m_id = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_id);
}

bool ShaderProgram::attachShader(const std::string& filename, GLenum shaderType) const
{
	REQUIRE(!filename.empty());
	REQUIRE(shaderType != NULL);

	// Create shader
	GLuint shader = glCreateShader(shaderType);

	// Load shader file
	std::string sourcestr{};
	if (!loadShader(filename, sourcestr))
		return false;
	const char* shaderSource = sourcestr.c_str();

	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	if (!validateShaderObject(shader, GL_COMPILE_STATUS))
	{
		glDeleteShader(shader);
		return false;
	}
	std::cout << "Shader compilation successful" << std::endl;

	// Attach shader to shader program and delete shader afterwards
	glAttachShader(m_id, shader);
	glDeleteShader(shader);

	std::cout << filename + " successfully attached to shaderprogram " << m_id << std::endl;

	glLinkProgram(m_id);

	// TODO Ensure that there is one more shader than at the beginning

	return validateShaderObject(m_id, GL_LINK_STATUS); // Validate program linking
}

bool ShaderProgram::validate() const
{
	if (!glIsProgram(m_id))
	{
		std::cout << "Shader program not valid" << std::endl;
		return false;
	}
	return validateShaderObject(m_id, GL_LINK_STATUS);
}

void ShaderProgram::use()
{
	glUseProgram(m_id);
}

void ShaderProgram::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void ShaderProgram::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

bool ShaderProgram::loadShader(const std::string& name, std::string& shaderSource) const
{
	REQUIRE(!name.empty());

	if (!shaderSource.empty())
		shaderSource.clear();

	std::ifstream file("../Data/Shaders/" + name);
	if (!file.is_open())
	{
		std::cout << "Could not open shader: " + name << std::endl;
		return false;
	}

	std::stringstream shaderData;
	shaderData << file.rdbuf();  // Loads the entire string into a string stream.
	file.close();
	shaderSource = std::move(shaderData.str());

	if (shaderSource.empty())
	{
		std::cout << "Empty shader file: " + name << std::endl;
		return false;
	}

	ENSURE(!shaderSource.empty());
	return true;
}

bool ShaderProgram::validateShaderObject(GLuint object, GLenum paramType) const
{
	if (object == 0)
	{
		std::cout << "Object not valid" << std::endl;
		return false;
	}

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(object, paramType, &success);
	if (!success)
	{
		switch (paramType)
		{
		case GL_COMPILE_STATUS:
			glGetShaderInfoLog(object, 512, NULL, infoLog);
			std::cout << "Shader compilation failed:\n" << infoLog << std::endl;
			break;
		case GL_LINK_STATUS:
			glGetProgramInfoLog(object, 512, NULL, infoLog);
			std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
			break;
		default:
			std::cout << "Undefined paramType in validateShaderObject()" << std::endl;
			break;
		}
		return false;
	}
	return true;
}