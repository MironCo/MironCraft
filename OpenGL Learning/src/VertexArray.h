#pragma once
#include "Common.h"
#include "VertexBuffer.h"

class VertexArray
{
public:
	GLuint vertexArrayID;
public:
	VertexArray()
	{
		//Create a vertex array object
		glGenVertexArrays(1, &vertexArrayID);
	}
	void Link(VertexBuffer &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
	{
		vbo.Bind();
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		vbo.Unbind();
	}
	void Bind()
	{
		glBindVertexArray(vertexArrayID);
	}
	void Unbind()
	{
		glBindVertexArray(0);
	}
	void Delete()
	{
		glDeleteVertexArrays(1, &vertexArrayID);
	}
};