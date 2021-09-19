#pragma once
#include "Common.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>

std::string getFileContents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

class Shader
{
public:
	GLuint shaderProgramID;
public:
	Shader() = default;
	Shader(const char* vertexFile, const char* fragmentFile)
	{
		std::string vertexCode = getFileContents(vertexFile);
		std::string fragmentCode = getFileContents(fragmentFile);

		// Convert the shader source strings into character arrays
		const char* vertexSource = vertexCode.c_str();
		const char* fragmentSource = fragmentCode.c_str();

		// Create Vertex Shader Object 
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);
		CheckForCompilerErrors(vertexShader, "VERTEX");

		// Create Fragment Shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);
		CheckForCompilerErrors(fragmentShader, "FRAGMENT");

		//Make Shader Program
		shaderProgramID = glCreateProgram();
		glAttachShader(shaderProgramID, vertexShader);
		glAttachShader(shaderProgramID, fragmentShader);
		glLinkProgram(shaderProgramID);
		CheckForCompilerErrors(shaderProgramID, "PROGRAM");

		// Delete the now useless Vertex and Fragment Shader objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	};
	void Activate()
	{
		glUseProgram(shaderProgramID);
	}
	void Delete()
	{
		glDeleteProgram(shaderProgramID);
	}

	void CheckForCompilerErrors(unsigned int shader, const char* type)
	{
		GLint hasCompiled;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "Shader Compliation Error for:" << type << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "Shader Linking Error for:" << type << std::endl;
			}
		}
	}
};