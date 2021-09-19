#pragma once
#include "Common.h"

class IndexBuffer
{
public:
	GLuint indexBufferID;
public:
	IndexBuffer()
	{
		glGenBuffers(1, &indexBufferID);
	}
	IndexBuffer(GLuint* _indices, GLsizeiptr _size)
	{
		//Generate one buffer and make IBO the reference
		glGenBuffers(1, &indexBufferID);
		//Make IBO an index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		//Add the data into the buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, _indices, GL_STATIC_DRAW);
	}
	void AddData(GLuint* _indices, GLintptr _offset, GLsizeiptr _size)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, _offset, _size, _indices);
		Unbind();
	}
	void Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	}
	void Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void Delete()
	{
		glDeleteBuffers(1, &indexBufferID);
	}
};