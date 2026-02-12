#include "IndexBuffer.h"

IndexBuffer::IndexBuffer()
{
	// Don't generate buffer in default constructor
}

IndexBuffer::IndexBuffer(GLuint* _indices, GLsizeiptr _size)
{
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, _indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	if (indexBufferID != 0)
	{
		glDeleteBuffers(1, &indexBufferID);
	}
}

void IndexBuffer::AddData(GLuint* _indices, GLintptr _offset, GLsizeiptr _size)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, _offset, _size, _indices);
	Unbind();
}

void IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
}

void IndexBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Delete()
{
	glDeleteBuffers(1, &indexBufferID);
}
