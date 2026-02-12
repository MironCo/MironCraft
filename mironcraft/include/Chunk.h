#pragma once
#include "Camera.h"
#include "Block.h"
#include "Tree.h"

#include <thread>
#include <future>

enum Biome
{
	GRASSLAND = 2, FOREST = 3, DESERT = 1, OCEAN = -1, HILLS = 4,
};

class Chunk
{
private:
	std::vector<std::vector<std::vector<Block*>>> blocksToRender;
	Biome biomeType = Biome::GRASSLAND;

	static const int CHUNK_SIZE = 16;
	static const int CHUNK_DEPTH = 6;

	int chunkX = 0;
	int chunkZ = 0;

	int randomOffset = 0;
	int divisor = 1;
	int amplitude = 4;

	int totalBlocks = 0;

	glm::mat4 model = glm::mat4(1.0f);
	VertexArray chunkVAO;
	VertexBuffer blocksVBO;
	IndexBuffer blocksIBO;
	Texture texture;
	bool hasGenerated = false;
public:
	bool isLoaded = false;
	Chunk(glm::vec2 _position, int _randomOffset, int _divisor, Shader& shader, Biome _biome);
	void DrawChunk(Shader& shader, Camera& cam);
	void BatchBlocks();
	void Generate();
	void CheckDistanceToCamera(Camera& _cam, Shader& shader);
};
