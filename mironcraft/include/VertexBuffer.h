#pragma once
#include "Common.h"

class VertexBuffer
{
public:
	GLuint vertexBufferId = 0;

	VertexBuffer();
	VertexBuffer(GLfloat* _vertices, GLsizeiptr _size);
	~VertexBuffer();

	// Disable copying to prevent double-free
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	void AddData(const void* _data, GLintptr _offset, GLsizeiptr _size);
	void Bind();
	void Unbind();
	void Delete();
};
