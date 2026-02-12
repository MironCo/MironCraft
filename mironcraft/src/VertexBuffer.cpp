#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
	// Don't generate buffer in default constructor - we'll do it manually when needed
}

VertexBuffer::VertexBuffer(GLfloat* _vertices, GLsizeiptr _size)
{
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, _size, _vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	if (vertexBufferId != 0)
	{
		glDeleteBuffers(1, &vertexBufferId);
	}
}

void VertexBuffer::AddData(const void* _data, GLintptr _offset, GLsizeiptr _size)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferSubData(GL_ARRAY_BUFFER, _offset, _size, _data);
}

void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
}

void VertexBuffer::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Delete()
{
	glDeleteBuffers(1, &vertexBufferId);
}
