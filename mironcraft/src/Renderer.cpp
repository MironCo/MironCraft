#include "Renderer.h"
#include "Chunk.h"
#include "Common.h"
#include <GLM/gtc/type_ptr.hpp>

std::vector<std::unique_ptr<Chunk>> Renderer::chunksToRender;
std::unique_ptr<ShadowMap> Renderer::shadowMap;

void Renderer::Init()
{
	shadowMap = std::make_unique<ShadowMap>(2048, 2048);
}

void Renderer::AddToRender(std::unique_ptr<Chunk> chunk)
{
	chunksToRender.push_back(std::move(chunk));
}

void Renderer::DrawShadowPass(Player& player)
{
	if (!shadowMap) return;

	// Update light matrix centered on player
	shadowMap->UpdateLightSpaceMatrix(player.position, 48.0f);

	// Bind shadow FBO
	shadowMap->BindForWriting();

	// Render all chunks with shadow shader
	shadowMap->shadowShader.Activate();
	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded)
		{
			chunk->DrawChunk(shadowMap->shadowShader);
		}
	}

	// Restore default framebuffer and viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WindowData::framebufferWidth, WindowData::framebufferHeight);
}

void Renderer::Draw(Shader& shader, Player& player)
{
	// First do shadow pass
	DrawShadowPass(player);

	// Update chunk loading
	for (auto& chunk : chunksToRender)
	{
		chunk->CheckDistanceToPlayer(player, shader);
	}

	// Set shadow uniforms on main shader
	shader.Activate();

	// Ensure texture unit 0 is set for the block texture
	glUniform1i(glGetUniformLocation(shader.shaderProgramID, "tex0"), 0);

	// Pass camera position for specular lighting
	glUniform3fv(
		glGetUniformLocation(shader.shaderProgramID, "viewPos"),
		1, glm::value_ptr(player.position)
	);

	if (shadowMap)
	{
		glUniformMatrix4fv(
			glGetUniformLocation(shader.shaderProgramID, "lightSpaceMatrix"),
			1, GL_FALSE, glm::value_ptr(shadowMap->lightSpaceMatrix)
		);
		glUniform3fv(
			glGetUniformLocation(shader.shaderProgramID, "lightDir"),
			1, glm::value_ptr(shadowMap->lightDir)
		);

		// Bind shadow map to texture unit 1
		shadowMap->BindForReading(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(shader.shaderProgramID, "shadowMap"), 1);
	}

	// Make sure we're back on texture unit 0 for the block textures
	glActiveTexture(GL_TEXTURE0);

	// Draw chunks
	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded)
		{
			chunk->DrawChunk(shader);
		}
	}
}
