#pragma once
#include <vector>

#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Chunk.h"

class Renderer
{
private:
	static std::vector<Chunk*> chunksToRender;
public:
	static inline void AddToRender(Chunk* &chunkToAdd)
	{
		chunksToRender.push_back(chunkToAdd);
	}
	static inline void Draw(Shader& _shader, Camera& _camera)
	{	
		for (Chunk* chunk : chunksToRender)
		{
			chunk->CheckDistanceToCamera(_camera, _shader);
			if (chunk->isLoaded)chunk->DrawChunk(_shader, _camera);
		}
	}
};

std::vector<Chunk*> Renderer::chunksToRender;