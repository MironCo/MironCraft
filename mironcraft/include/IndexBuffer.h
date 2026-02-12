#pragma once
#include "Common.h"

class IndexBuffer
{
public:
	GLuint indexBufferID = 0;

	IndexBuffer();
	IndexBuffer(GLuint* _indices, GLsizeiptr _size);
	~IndexBuffer();

	// Disable copying to prevent double-free
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	void AddData(GLuint* _indices, GLintptr _offset, GLsizeiptr _size);
	void Bind();
	void Unbind();
	void Delete();
};
