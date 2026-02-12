#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &vertexArrayID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &vertexArrayID);
}

void VertexArray::Link(VertexBuffer& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	vbo.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	vbo.Unbind();
}

void VertexArray::Bind()
{
	glBindVertexArray(vertexArrayID);
}

void VertexArray::Unbind()
{
	glBindVertexArray(0);
}

void VertexArray::Delete()
{
	glDeleteVertexArrays(1, &vertexArrayID);
}
