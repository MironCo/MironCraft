#pragma once
#include <glad/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "Shader.h"

class ShadowMap
{
private:
	GLuint depthMapFBO;
	GLuint depthMap;
	unsigned int shadowWidth;
	unsigned int shadowHeight;

public:
	Shader shadowShader;
	glm::vec3 lightDir;
	glm::mat4 lightSpaceMatrix;

	ShadowMap(unsigned int width = 2048, unsigned int height = 2048);
	~ShadowMap();

	void BindForWriting();
	void BindForReading(GLenum textureUnit);
	void UpdateLightSpaceMatrix(const glm::vec3& center, float orthoSize = 64.0f);
	void RotateLight(float angle);

	GLuint GetDepthMap() const { return depthMap; }
	unsigned int GetWidth() const { return shadowWidth; }
	unsigned int GetHeight() const { return shadowHeight; }
};
