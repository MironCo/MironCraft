#pragma once
#include "Common.h"

class VertexBuffer
{
public:
	GLuint vertexBufferId;
public:
	VertexBuffer()
	{
		glGenBuffers(1, &vertexBufferId);
	};
	VertexBuffer(GLfloat* _vertices, GLsizeiptr _size)
	{
		//Generate one buffer and make VBO the reference
		glGenBuffers(1, &vertexBufferId);
		//Make VBO a vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

		//Add the data into the buffer
		glBufferData(GL_ARRAY_BUFFER, _size, _vertices, GL_STATIC_DRAW);
	}
	void AddData(const void* _data, GLintptr _offset, GLsizeiptr _size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferSubData(GL_ARRAY_BUFFER, _offset, _size, _data);
	}
	void Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	}
	void Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void Delete()
	{
		glDeleteBuffers(1, &vertexBufferId);
	}
};