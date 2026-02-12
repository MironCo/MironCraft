#pragma once
#include "Common.h"
#include "VertexBuffer.h"

class VertexArray
{
public:
	GLuint vertexArrayID = 0;

	VertexArray();
	~VertexArray();

	void Link(VertexBuffer& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void Bind();
	void Unbind();
	void Delete();
};
