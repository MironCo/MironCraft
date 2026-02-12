#pragma once
#include "Common.h"
#include "Shader.h"

#include <stb/stb_image.h>

class Texture
{
public:
	GLuint textureID;
	GLenum type;
public:
	Texture() = default;
	Texture(const char* image, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType)
	{
		type = textureType;
		//Texture Stuff
		int imageWidth, imageHeight, numberColorChannels;
		//stbi_set_flip_vertically_on_load(true);
		unsigned char* imageBytes = stbi_load(image, &imageWidth, &imageHeight, &numberColorChannels, 0);

		glGenTextures(1, &textureID);
		glActiveTexture(slot);
		glBindTexture(textureType, textureID);

		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(textureType, 0, GL_RGBA, imageWidth, imageHeight, 0, format, pixelType, imageBytes);
		glGenerateMipmap(textureType);

		stbi_image_free(imageBytes);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void TextureUniform(Shader &shader, const char* uniform, GLuint uniformID)
	{
		shader.Activate();
		GLuint texUniform = glGetUniformLocation(shader.shaderProgramID, "tex0");
		glUniform1i(texUniform, uniformID);
	}
	void SetTextureUniformOffset(Shader& shader, int offsetX, int offsetY)
	{
		shader.Activate();
		GLuint texUniform = glGetUniformLocation(shader.shaderProgramID, "textureOffset");

		GLfloat offX = (0.25f * offsetX);
		GLfloat offY = (0.25f * offsetY);

		glUniform2f(texUniform, offX, offY);
	}
	void Bind()
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	void Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Delete()
	{
		glDeleteTextures(1, &textureID);
	}
};