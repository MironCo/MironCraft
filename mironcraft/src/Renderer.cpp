#include "Renderer.h"
#include "Chunk.h"

std::vector<std::unique_ptr<Chunk>> Renderer::chunksToRender;

void Renderer::AddToRender(std::unique_ptr<Chunk> chunk)
{
	chunksToRender.push_back(std::move(chunk));
}

void Renderer::Draw(Shader& shader, Player& player)
{
	for (auto& chunk : chunksToRender)
	{
		chunk->CheckDistanceToPlayer(player, shader);
		if (chunk->isLoaded)
		{
			chunk->DrawChunk(shader);
		}
	}
}
