#pragma once
#include "Common.h"
#include "Shader.h"

class Texture
{
public:
	GLuint textureID = 0;
	GLenum type = GL_TEXTURE_2D;

	Texture() = default;
	Texture(const char* image, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType);

	void TextureUniform(Shader& shader, const char* uniform, GLuint uniformID);
	void SetTextureUniformOffset(Shader& shader, int offsetX, int offsetY);
	void Bind();
	void Unbind();
	void Delete();
};
