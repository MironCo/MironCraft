#include "Renderer.h"
#include "Chunk.h"
#include "Common.h"
#include "Collision.h"
#include <GLM/gtc/type_ptr.hpp>

std::vector<std::unique_ptr<Chunk>> Renderer::chunksToRender;
std::unique_ptr<ShadowMap> Renderer::shadowMap;
std::unique_ptr<Shader> Renderer::outlineShader;

void Renderer::Init()
{
	shadowMap = std::make_unique<ShadowMap>(2048, 2048);
	outlineShader = std::make_unique<Shader>("res/shaders/outline.vert", "res/shaders/outline.frag");
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

void Renderer::DrawOutlinePass(Player& player)
{
	if (!outlineShader) return;

	// Enable culling just for outline pass - render front faces in black (slightly expanded)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	outlineShader->Activate();

	// Set camera matrix
	player.Matrix(60.0f, 0.02f, 100.0f, *outlineShader, "cameraMatrix");

	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded)
		{
			chunk->DrawChunk(*outlineShader);
		}
	}

	// Disable culling for main pass
	glDisable(GL_CULL_FACE);
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

	// Draw outlines first (back faces, black)
	DrawOutlinePass(player);

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

	// Draw opaque chunks first
	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded)
		{
			chunk->DrawChunk(shader);
		}
	}

	// Draw water with transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // Don't write to depth buffer for transparent objects

	glUniform1i(glGetUniformLocation(shader.shaderProgramID, "isWater"), 1);

	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded)
		{
			chunk->DrawWater(shader);
		}
	}

	glUniform1i(glGetUniformLocation(shader.shaderProgramID, "isWater"), 0);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void Renderer::RemoveBlock(int worldX, int worldY, int worldZ)
{
	// Remove from collision world
	g_CollisionWorld.RemoveBlock(worldX, worldY, worldZ);

	// Find the chunk containing this block and update its mesh
	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded && chunk->RemoveBlockAtWorld(worldX, worldY, worldZ))
		{
			chunk->RebuildMesh();
			break;
		}
	}
}

void Renderer::RotateLight(float angle)
{
	if (shadowMap)
	{
		shadowMap->RotateLight(angle);
	}
}
