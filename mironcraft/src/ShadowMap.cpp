#include "ShadowMap.h"
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/rotate_vector.hpp>

ShadowMap::ShadowMap(unsigned int width, unsigned int height)
	: shadowWidth(width)
	, shadowHeight(height)
	, shadowShader("res/shaders/shadow.vert", "res/shaders/shadow.frag")
	, lightDir(glm::normalize(glm::vec3(-0.8f, -0.5f, -0.4f)))
{
	// Create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Create framebuffer
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap()
{
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
}

void ShadowMap::BindForWriting()
{
	glViewport(0, 0, shadowWidth, shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::BindForReading(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

void ShadowMap::UpdateLightSpaceMatrix(const glm::vec3& center, float orthoSize)
{
	// Position light "above" the center looking down along lightDir
	float lightDistance = 50.0f;
	glm::vec3 lightPos = center - lightDir * lightDistance;

	glm::mat4 lightProjection = glm::ortho(
		-orthoSize, orthoSize,
		-orthoSize, orthoSize,
		1.0f, lightDistance * 2.0f
	);

	glm::mat4 lightView = glm::lookAt(
		lightPos,
		center,
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	lightSpaceMatrix = lightProjection * lightView;

	// Update shader uniform
	shadowShader.Activate();
	glUniformMatrix4fv(
		glGetUniformLocation(shadowShader.shaderProgramID, "lightSpaceMatrix"),
		1, GL_FALSE, glm::value_ptr(lightSpaceMatrix)
	);
}

void ShadowMap::RotateLight(float angle)
{
	// Rotate light direction around the Y axis
	lightDir = glm::rotateY(lightDir, glm::radians(angle));
	lightDir = glm::normalize(lightDir);
}
