#pragma once
#include <vector>
#include <memory>

#include "Shader.h"
#include "Player.h"
#include "ShadowMap.h"

class Chunk;

class Renderer
{
private:
	static std::vector<std::unique_ptr<Chunk>> chunksToRender;
	static std::unique_ptr<ShadowMap> shadowMap;
public:
	static void Init();
	static void AddToRender(std::unique_ptr<Chunk> chunk);
	static void Draw(Shader& shader, Player& player);
	static void DrawShadowPass(Player& player);
};
