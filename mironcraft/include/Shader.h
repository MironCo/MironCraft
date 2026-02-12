#pragma once
#include "Common.h"
#include <string>

std::string getFileContents(const char* filename);

class Shader
{
public:
	GLuint shaderProgramID = 0;

	Shader() = default;
	Shader(const char* vertexFile, const char* fragmentFile);

	void Activate();
	void Delete();
	void CheckForCompilerErrors(unsigned int shader, const char* type);
};
