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
	static std::unique_ptr<Shader> outlineShader;
public:
	static void Init();
	static void AddToRender(std::unique_ptr<Chunk> chunk);
	static void Draw(Shader& shader, Player& player);
	static void DrawShadowPass(Player& player);
	static void DrawOutlinePass(Player& player);

	// Remove a block at world position and rebuild affected chunk
	static void RemoveBlock(int worldX, int worldY, int worldZ);

	// Rotate the sun/light
	static void RotateLight(float angle);
};
