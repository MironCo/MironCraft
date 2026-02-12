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
	static void AddToRender(Chunk* &chunkToAdd);
	static void Draw(Shader& _shader, Camera& _camera);
};
