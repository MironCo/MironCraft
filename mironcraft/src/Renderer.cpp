#include "Renderer.h"

std::vector<Chunk*> Renderer::chunksToRender;

void Renderer::AddToRender(Chunk* &chunkToAdd)
{
	chunksToRender.push_back(chunkToAdd);
}

void Renderer::Draw(Shader& _shader, Camera& _camera)
{
	for (Chunk* chunk : chunksToRender)
	{
		chunk->CheckDistanceToCamera(_camera, _shader);
		if (chunk->isLoaded) chunk->DrawChunk(_shader, _camera);
	}
}
