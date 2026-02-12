#pragma once
#include "Player.h"
#include "Block.h"

#include <memory>
#include <vector>

enum Biome
{
	GRASSLAND = 2, FOREST = 3, DESERT = 1, OCEAN = -1, HILLS = 4,
};

class Chunk
{
private:
	std::vector<std::vector<std::vector<std::unique_ptr<Block>>>> blocksToRender;
	Biome biomeType = Biome::GRASSLAND;

	static const int CHUNK_SIZE = 16;
	static const int CHUNK_DEPTH = 6;

	int chunkX = 0;
	int chunkZ = 0;

	int randomOffset = 0;
	int divisor = 1;
	int amplitude = 4;

	int totalIndices = 0;
	int totalWaterIndices = 0;

	glm::mat4 model = glm::mat4(1.0f);
	VertexArray chunkVAO;
	VertexBuffer blocksVBO;
	IndexBuffer blocksIBO;

	// Separate buffers for water (transparent)
	VertexArray waterVAO;
	VertexBuffer waterVBO;
	IndexBuffer waterIBO;

	Texture texture;
	bool hasGenerated = false;

public:
	bool isLoaded = false;

	Chunk(glm::vec2 position, int randomOffset, int divisor, Shader& shader, Biome biome);
	void DrawChunk(Shader& shader);
	void DrawWater(Shader& shader);
	void BatchBlocks();
	void Generate();
	void CheckDistanceToPlayer(Player& player, Shader& shader);

	// Remove a block at world position, returns true if block was in this chunk
	bool RemoveBlockAtWorld(int worldX, int worldY, int worldZ);
	void RebuildMesh();

	// Get chunk boundaries
	int GetChunkX() const { return chunkX; }
	int GetChunkZ() const { return chunkZ; }
	static int GetChunkSize() { return CHUNK_SIZE; }

private:
	bool HasSolidBlockAt(int x, int z, size_t y) const;
	bool HasBlockAt(int x, int z, size_t y) const;
};
