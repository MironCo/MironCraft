#pragma once
#include <vector>
#include <memory>

#include "Shader.h"
#include "Player.h"

class Chunk;

class Renderer
{
private:
	static std::vector<std::unique_ptr<Chunk>> chunksToRender;
public:
	static void AddToRender(std::unique_ptr<Chunk> chunk);
	static void Draw(Shader& shader, Player& player);
};
