#include "Renderer.h"
#include "Chunk.h"

std::vector<std::unique_ptr<Chunk>> Renderer::chunksToRender;

void Renderer::AddToRender(std::unique_ptr<Chunk> chunk)
{
	chunksToRender.push_back(std::move(chunk));
}

void Renderer::Draw(Shader& shader, Camera& camera)
{
	for (auto& chunk : chunksToRender)
	{
		chunk->CheckDistanceToCamera(camera, shader);
		if (chunk->isLoaded)
		{
			chunk->DrawChunk(shader, camera);
		}
	}
}
