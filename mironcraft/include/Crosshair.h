#pragma once
#include <glad/glad.h>
#include "Shader.h"

class Crosshair
{
private:
	GLuint VAO, VBO;
	Shader shader;

public:
	Crosshair();
	~Crosshair();
	void Draw();
};
