#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

#pragma warning (push, 2)  // Temporarily set warning level 2
#include <3rdParty/glm/gtc/type_ptr.hpp>
#pragma warning (pop)      // Restore back

#include "Renderer/shaderprogram.h"
#include "Utility/contract.h"

ShaderProgram::ShaderProgram() { m_id = glCreateProgram(); }

ShaderProgram::~ShaderProgram() { glDeleteProgram(m_id); }

GLuint ShaderProgram::getID() const { return m_id; }

bool ShaderProgram::attachShader(const std::string& filename, GLenum shaderType) const
{
	REQUIRE(!filename.empty());
	REQUIRE(shaderType != NULL);


	std::cout << "\tStarted attaching shader" << std::endl;

	// Create shader
	const GLuint shader = glCreateShader(shaderType);

	// Load shader file
	std::string sourcestr{};
	if (!loadShader(filename, sourcestr))
		return false;
	const char* sourcechar = sourcestr.c_str();

	glShaderSource(shader, 1, &sourcechar, NULL);
	glCompileShader(shader);
	if (!validateShaderObject(shader, GL_COMPILE_STATUS)) {
		glDeleteShader(shader);
		return false;
	}
	std::cout << "\tShader compilation successful" << std::endl;

	// Attach shader to shader program and delete shader afterwards
	glAttachShader(m_id, shader);
	glDeleteShader(shader);

	std::cout << "\t" << filename + " successfully attached to shaderprogram " << m_id << std::endl;

	glLinkProgram(m_id);

	// TODO Ensure that there is one more shader than at the beginning

	return validateShaderObject(m_id, GL_LINK_STATUS); // Validate program linking
}

bool ShaderProgram::validate() const
{
	if (!glIsProgram(m_id)) {
		std::cout << "\tShader program not valid" << std::endl;
		return false;
	}
	return validateShaderObject(m_id, GL_LINK_STATUS);
}

void ShaderProgram::use() const { glUseProgram(m_id); }

void ShaderProgram::setBool(const std::string& name, bool value) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetBool" << std::endl;
		return;
	}

	setInt(name, static_cast<int>(value));
}

void ShaderProgram::setInt(const std::string& name, int value) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetInt" << std::endl;
		return;
	}

	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetFloat" << std::endl;
		return;
	}

	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetBool" << std::endl;
		return;
	}

	glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec2(const std::string& name, float x, float y) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetVec2" << std::endl;
		return;
	}

	glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetVec3" << std::endl;
		return;
	}

	glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetVec3" << std::endl;
		return;
	}

	glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetVec4" << std::endl;
		return;
	}

	glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetVec4" << std::endl;
		return;
	}

	glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
}

void ShaderProgram::setMat2(const std::string& name, const glm::mat2& mat) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetMat2" << std::endl;
		return;
	}

	glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setMat3(const std::string& name, const glm::mat3& mat) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetMat3" << std::endl;
		return;
	}

	glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "Invalid name in SetMat4" << std::endl;
		return;
	}

	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

bool ShaderProgram::loadShader(const std::string& name, std::string& shaderSource) const
{
	REQUIRE(!name.empty());
	if (name.empty()) {
		std::cerr << "\tInvalid name in loadShader" << std::endl;
		return false;
	}

	std::cout << "\tLoading shader file: " << name << std::endl;

	if (!shaderSource.empty())
		shaderSource.clear();

	std::ifstream file("../Data/Shaders/" + name);
	if (!file.is_open()) {
		std::cerr << "\tCould not open shader: " + name << std::endl;
		return false;
	}

	std::stringstream shaderData;
	shaderData << file.rdbuf(); // Loads the entire string into a stringstream.

	// Get file length
	const std::streamoff filesize = file.tellg();
	file.close();

	// Get contents to string and count new lines in it to compare it to file length
	shaderSource = std::move(shaderData.str());
	const unsigned int newLines = static_cast<unsigned int>(std::count(shaderSource.begin(), shaderSource.end(), '\n'));

	ENSURE(!shaderSource.empty());
	ENSURE(shaderSource.size() + newLines == static_cast<unsigned int>(filesize));

	if (shaderSource.empty()) {
		std::cerr << "\tEmpty shader file: " + name << std::endl;
		return false;
	}
	if (shaderSource.size() + newLines != static_cast<unsigned int>(filesize)) {
		std::cerr << "\tShader loaded does not match shader file: " + name << std::endl;
		return false;
	}
	std::cout << "\tShader file loaded successfully" << std::endl;

	return true;
}

bool ShaderProgram::validateShaderObject(GLuint object, GLenum paramType)
{
	REQUIRE(object != 0);
	if (object == 0) {
		std::cerr << "\tObject not valid" << std::endl;
		return false;
	}

	GLint success;
	GLchar infoLog[1024];
	if (paramType != GL_LINK_STATUS) {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR: \n\t" << infoLog << std::endl;
			return false;
		}
		return true;
	}
	else {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR: \n\t" << infoLog << std::endl;
			return false;
		}
		return true;
	}
}
