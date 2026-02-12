#pragma once
#include "Player.h"
#include "Block.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <memory>
#include <vector>
#include <atomic>

enum Biome
{
	GRASSLAND = 2, FOREST = 3, DESERT = 1, OCEAN = -1, HILLS = 4,
};

// CPU-side mesh data that can be built on worker threads
struct ChunkMeshData
{
	std::vector<GLfloat> blockVerts;
	std::vector<GLuint> blockInds;
	std::vector<GLfloat> waterVerts;
	std::vector<GLuint> waterInds;

	// Collision data to add on main thread
	std::vector<glm::ivec3> collisionBlocks;
};

enum class ChunkState
{
	Unloaded,
	Generating,      // Worker thread is generating blocks
	Generated,       // Blocks generated, ready for mesh building
	BuildingMesh,    // Worker thread is building mesh
	ReadyForUpload,  // Mesh built, waiting for GPU upload
	Ready            // Fully loaded and renderable
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

	// Threading state
	std::atomic<ChunkState> state{ChunkState::Unloaded};
	ChunkMeshData meshData;  // CPU-side mesh data

public:
	bool isLoaded = false;

	Chunk(glm::vec2 position, int randomOffset, int divisor, Shader& shader, Biome biome);
	void DrawChunk(Shader& shader);
	void DrawWater(Shader& shader);
	void Generate();
	void CheckDistanceToPlayer(Player& player, Shader& shader);

	// Threaded mesh building
	void BuildMeshData();      // CPU work - can run on worker thread
	void UploadMeshToGPU();    // GPU work - must run on main thread
	void ApplyCollision();     // Apply collision data - must run on main thread

	// State management
	ChunkState GetState() const { return state.load(); }
	void SetState(ChunkState s) { state.store(s); }

	// Check if chunk should start loading based on distance
	bool ShouldStartLoading(const Player& player) const;
	bool ShouldBuildMesh() const;

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
