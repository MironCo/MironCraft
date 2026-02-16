#include "Chunk.h"
#include "Tree.h"
#include "Collision.h"
#include <GLM/gtc/noise.hpp>
#include <cstdlib>

Chunk::Chunk(glm::vec2 position, int _randomOffset, int _divisor, Shader& shader, Biome biome)
	: biomeType(biome)
	, chunkX(static_cast<int>(position.x))
	, chunkZ(static_cast<int>(position.y))
	, randomOffset(_randomOffset)
	, divisor(_divisor)
	, texture("res/textures/texture_Atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE)
{
	switch (biomeType)
	{
		case Biome::OCEAN:
			amplitude = 3;
			break;
		case Biome::HILLS:
			amplitude = 10;
			break;
		default:
			amplitude = 4;
			break;
	}
}

void Chunk::DrawChunk(Shader& shader)
{
	shader.Activate();
	chunkVAO.Bind();
	texture.Bind();
	glDrawElements(GL_TRIANGLES, totalIndices, GL_UNSIGNED_INT, 0);
}

void Chunk::DrawWater(Shader& shader)
{
	if (totalWaterIndices == 0) return;

	shader.Activate();
	waterVAO.Bind();
	texture.Bind();
	glDrawElements(GL_TRIANGLES, totalWaterIndices, GL_UNSIGNED_INT, 0);
}

// Helper to add face vertices and indices
static void AddFace(int face, GLfloat* blockVertices, int& currentVertex,
                    std::vector<GLfloat>& verts, std::vector<GLuint>& inds)
{
	int vertStart = face * 32;
	for (int v = 0; v < 32; v++)
	{
		verts.push_back(blockVertices[vertStart + v]);
	}

	if (face < 3) // Front, Back, Left
	{
		inds.push_back(currentVertex + 0);
		inds.push_back(currentVertex + 1);
		inds.push_back(currentVertex + 2);
		inds.push_back(currentVertex + 0);
		inds.push_back(currentVertex + 2);
		inds.push_back(currentVertex + 3);
	}
	else if (face == 3) // Right
	{
		inds.push_back(currentVertex + 0);
		inds.push_back(currentVertex + 1);
		inds.push_back(currentVertex + 2);
		inds.push_back(currentVertex + 2);
		inds.push_back(currentVertex + 3);
		inds.push_back(currentVertex + 1);
	}
	else // Top/Bottom
	{
		inds.push_back(currentVertex + 0);
		inds.push_back(currentVertex + 1);
		inds.push_back(currentVertex + 2);
		inds.push_back(currentVertex + 2);
		inds.push_back(currentVertex + 3);
		inds.push_back(currentVertex + 1);
	}

	currentVertex += 4;
}

// CPU-side mesh building - can run on worker thread (no OpenGL calls!)
void Chunk::BuildMeshData()
{
	// Clear previous data
	meshData.blockVerts.clear();
	meshData.blockInds.clear();
	meshData.waterVerts.clear();
	meshData.waterInds.clear();
	meshData.collisionBlocks.clear();

	int currentVertex = 0;
	int currentWaterVertex = 0;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (size_t y = 0; y < blocksToRender[x][z].size(); y++)
			{
				auto& block = blocksToRender[x][z][y];
				if (block)
				{
					GLfloat* blockVertices = block->GetBlockVertsWithOffset(
						block->position, block->textureOffset, block->GetBlockType());

					bool isWater = (block->GetBlockType() == BlockType::WATER);
					bool isTreeBlock = (block->GetBlockType() == BlockType::LOG ||
					                    block->GetBlockType() == BlockType::LEAVES);

					bool faces[6];
					if (isTreeBlock)
					{
						for (int i = 0; i < 6; i++) faces[i] = true;
					}
					else if (isWater)
					{
						// Water: cull faces adjacent to ANY block (water or solid)
						// Only show faces adjacent to air
						faces[0] = !HasBlockAt(x, z + 1, y);
						faces[1] = !HasBlockAt(x, z - 1, y);
						faces[2] = !HasBlockAt(x - 1, z, y);
						faces[3] = !HasBlockAt(x + 1, z, y);
						faces[4] = !HasBlockAt(x, z, y + 1); // Only show top if nothing above
						faces[5] = false; // Never show bottom
					}
					else
					{
						faces[0] = !HasSolidBlockAt(x, z + 1, y);
						faces[1] = !HasSolidBlockAt(x, z - 1, y);
						faces[2] = !HasSolidBlockAt(x - 1, z, y);
						faces[3] = !HasSolidBlockAt(x + 1, z, y);
						faces[4] = !HasSolidBlockAt(x, z, y + 1);
						faces[5] = (y == 0) || !HasSolidBlockAt(x, z, y - 1);
					}

					for (int face = 0; face < 6; face++)
					{
						if (faces[face])
						{
							if (isWater)
							{
								AddFace(face, blockVertices, currentWaterVertex, meshData.waterVerts, meshData.waterInds);
							}
							else
							{
								AddFace(face, blockVertices, currentVertex, meshData.blockVerts, meshData.blockInds);
							}
						}
					}

					// Track collision blocks (non-water only)
					if (!isWater)
					{
						meshData.collisionBlocks.push_back(glm::ivec3(
							static_cast<int>(block->position.x),
							static_cast<int>(block->position.y),
							static_cast<int>(block->position.z)));
					}
				}
			}
		}
	}
}

// GPU-side mesh upload - MUST run on main thread
void Chunk::UploadMeshToGPU()
{
	// Setup solid block mesh
	totalIndices = static_cast<int>(meshData.blockInds.size());

	chunkVAO.Bind();

	glGenBuffers(1, &blocksVBO.vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, blocksVBO.vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, meshData.blockVerts.size() * sizeof(float), meshData.blockVerts.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &blocksIBO.indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blocksIBO.indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.blockInds.size() * sizeof(int), meshData.blockInds.data(), GL_STATIC_DRAW);

	chunkVAO.Link(blocksVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	chunkVAO.Link(blocksVBO, 1, 1, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	chunkVAO.Link(blocksVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(4 * sizeof(float)));
	chunkVAO.Link(blocksVBO, 3, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	chunkVAO.Unbind();
	blocksVBO.Unbind();
	blocksIBO.Unbind();

	// Setup water mesh
	totalWaterIndices = static_cast<int>(meshData.waterInds.size());

	if (totalWaterIndices > 0)
	{
		waterVAO.Bind();

		glGenBuffers(1, &waterVBO.vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBO.vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, meshData.waterVerts.size() * sizeof(float), meshData.waterVerts.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &waterIBO.indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterIBO.indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.waterInds.size() * sizeof(int), meshData.waterInds.data(), GL_STATIC_DRAW);

		waterVAO.Link(waterVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		waterVAO.Link(waterVBO, 1, 1, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		waterVAO.Link(waterVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(4 * sizeof(float)));
		waterVAO.Link(waterVBO, 3, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		waterVAO.Unbind();
		waterVBO.Unbind();
		waterIBO.Unbind();
	}

	// Clear CPU-side data to free memory
	meshData.blockVerts.clear();
	meshData.blockVerts.shrink_to_fit();
	meshData.blockInds.clear();
	meshData.blockInds.shrink_to_fit();
	meshData.waterVerts.clear();
	meshData.waterVerts.shrink_to_fit();
	meshData.waterInds.clear();
	meshData.waterInds.shrink_to_fit();
}

// Apply collision data - MUST run on main thread (uses global collision world)
void Chunk::ApplyCollision()
{
	for (const auto& pos : meshData.collisionBlocks)
	{
		g_CollisionWorld.AddBlock(pos.x, pos.y, pos.z);
	}
	meshData.collisionBlocks.clear();
	meshData.collisionBlocks.shrink_to_fit();
}

// Get biome parameters for blending
struct BiomeParams
{
	float amplitude;
	float baseHeight;
	bool hasGrass;
	bool hasSand;
	bool hasDirt;
	bool canHaveTrees;
	bool canHaveWater;
	int treeChance;
};

static BiomeParams GetBiomeParams(Biome biome)
{
	BiomeParams p;
	switch (biome)
	{
		case Biome::OCEAN:
			p.amplitude = 3.0f;
			p.baseHeight = -1.0f;
			p.hasGrass = false;
			p.hasSand = true;
			p.hasDirt = false;
			p.canHaveTrees = false;
			p.canHaveWater = true;
			p.treeChance = 0;
			break;
		case Biome::DESERT:
			p.amplitude = 4.0f;
			p.baseHeight = 1.0f;
			p.hasGrass = false;
			p.hasSand = true;
			p.hasDirt = false;
			p.canHaveTrees = false;
			p.canHaveWater = false;
			p.treeChance = 0;
			break;
		case Biome::GRASSLAND:
			p.amplitude = 4.0f;
			p.baseHeight = 2.0f;
			p.hasGrass = true;
			p.hasSand = false;
			p.hasDirt = true;
			p.canHaveTrees = false;
			p.canHaveWater = true;
			p.treeChance = 220;
			break;
		case Biome::FOREST:
			p.amplitude = 4.0f;
			p.baseHeight = 3.0f;
			p.hasGrass = true;
			p.hasSand = false;
			p.hasDirt = true;
			p.canHaveTrees = true;
			p.canHaveWater = true;
			p.treeChance = 70;
			break;
		case Biome::HILLS:
			p.amplitude = 10.0f;
			p.baseHeight = 4.0f;
			p.hasGrass = true;
			p.hasSand = false;
			p.hasDirt = false;
			p.canHaveTrees = true;
			p.canHaveWater = true;
			p.treeChance = 220;
			break;
		default:
			p.amplitude = 4.0f;
			p.baseHeight = 2.0f;
			p.hasGrass = true;
			p.hasSand = false;
			p.hasDirt = true;
			p.canHaveTrees = false;
			p.canHaveWater = true;
			p.treeChance = 220;
			break;
	}
	return p;
}

// Check if a position should be carved out as a cave
static bool ShouldCarveCave(float worldX, float worldY, float worldZ, int seed, int surfaceHeight)
{
	// Worm tunnel noise - creates elongated tunnels using "spaghetti caves" technique
	// Higher frequency = smaller, tighter tunnels
	float wormScale = 0.1f;

	// Two perpendicular worm noises that intersect to form tunnels
	float worm1 = glm::perlin(glm::vec3(
		worldX * wormScale,
		worldY * wormScale * 0.7f,
		worldZ * wormScale
	) + glm::vec3(seed * 0.1f));

	float worm2 = glm::perlin(glm::vec3(
		worldX * wormScale + 100.0f,
		worldY * wormScale * 0.7f + 100.0f,
		worldZ * wormScale + 100.0f
	) + glm::vec3(seed * 0.1f));

	// Spaghetti cave formula: both noises must be near zero
	// This creates thin, winding tunnels instead of massive chambers
	float tunnelRadius = 0.15f;  // Smaller = thinner tunnels
	bool isWormCave = (glm::abs(worm1) < tunnelRadius) && (glm::abs(worm2) < tunnelRadius);

	// Rare larger chambers using a separate noise
	float chamberScale = 0.05f;
	float chamberNoise = glm::perlin(glm::vec3(
		worldX * chamberScale,
		worldY * chamberScale,
		worldZ * chamberScale
	) + glm::vec3(seed * 0.1f + 500.0f));

	// Only create chambers occasionally (high threshold)
	bool isChamber = chamberNoise > 0.55f;

	float depthBelowSurface = static_cast<float>(surfaceHeight) - worldY;

	// Surface entrance noise - determines where holes can appear
	float entranceNoise = glm::perlin(glm::vec3(
		worldX * 0.07f + seed * 0.1f,
		worldZ * 0.07f + seed * 0.15f,
		0.0f
	));

	// Allow surface holes where entrance noise is high AND there's a cave below
	if (depthBelowSurface >= 0.0f && depthBelowSurface < 3.0f)
	{
		// Only carve surface/near-surface if both cave exists AND entrance noise allows it
		if (isWormCave && entranceNoise > 0.25f)
			return true;
		return false;
	}

	// Deeper caves - normal generation
	return isWormCave || isChamber;
}

// Get biome weights at a world position (for smooth blending)
static void GetBiomeWeights(float worldX, float worldZ, int seed, float weights[5], Biome biomes[5])
{
	biomes[0] = Biome::GRASSLAND;
	biomes[1] = Biome::FOREST;
	biomes[2] = Biome::DESERT;
	biomes[3] = Biome::OCEAN;
	biomes[4] = Biome::HILLS;

	float biomeScale = 0.05f;
	float moisture = glm::simplex(glm::vec2(
		(worldX / 16.0f + seed) * biomeScale,
		(worldZ / 16.0f + seed) * biomeScale
	));
	float temperature = glm::simplex(glm::vec2(
		(worldX / 16.0f + seed + 1000) * biomeScale,
		(worldZ / 16.0f + seed + 1000) * biomeScale
	));

	// Calculate soft weights based on distance from biome centers in climate space
	// Desert: high temp, low moisture
	weights[2] = glm::clamp((temperature - 0.1f) * 2.0f, 0.0f, 1.0f) *
	             glm::clamp((-moisture - 0.0f) * 2.0f, 0.0f, 1.0f);

	// Ocean: low temp or low moisture
	float oceanTemp = glm::clamp((-temperature - 0.2f) * 2.0f, 0.0f, 1.0f);
	float oceanMoist = glm::clamp((-moisture - 0.3f) * 2.0f, 0.0f, 1.0f);
	weights[3] = glm::max(oceanTemp, oceanMoist);

	// Forest: high moisture
	weights[1] = glm::clamp((moisture - 0.1f) * 2.0f, 0.0f, 1.0f) *
	             (1.0f - weights[3]) * (1.0f - weights[2]);

	// Hills: medium-high temp, medium moisture
	weights[4] = glm::clamp((temperature + 0.2f) * 1.5f, 0.0f, 1.0f) *
	             glm::clamp((moisture + 0.4f) * 1.5f, 0.0f, 1.0f) *
	             (1.0f - weights[1]) * (1.0f - weights[2]) * (1.0f - weights[3]);

	// Grassland: fills the rest
	float total = weights[1] + weights[2] + weights[3] + weights[4];
	weights[0] = glm::max(0.0f, 1.0f - total);

	// Normalize weights
	total = weights[0] + weights[1] + weights[2] + weights[3] + weights[4];
	if (total > 0.0f)
	{
		for (int i = 0; i < 5; i++)
			weights[i] /= total;
	}
}

void Chunk::Generate()
{
	int chunkOffsetX = chunkX * CHUNK_SIZE;
	int chunkOffsetZ = chunkZ * CHUNK_SIZE;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		std::vector<std::vector<std::unique_ptr<Block>>> vecZY;
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			int worldX = x + chunkOffsetX;
			int worldZ = z + chunkOffsetZ;

			// Get biome weights at this position
			float weights[5];
			Biome biomes[5];
			GetBiomeWeights(static_cast<float>(worldX), static_cast<float>(worldZ), randomOffset, weights, biomes);

			// Blend terrain parameters
			float blendedAmplitude = 0.0f;
			float blendedBaseHeight = 0.0f;
			float grassWeight = 0.0f;
			float sandWeight = 0.0f;
			float dirtWeight = 0.0f;
			float treeWeight = 0.0f;
			float waterWeight = 0.0f;
			float avgTreeChance = 0.0f;

			for (int i = 0; i < 5; i++)
			{
				if (weights[i] > 0.001f)
				{
					BiomeParams p = GetBiomeParams(biomes[i]);
					blendedAmplitude += p.amplitude * weights[i];
					blendedBaseHeight += p.baseHeight * weights[i];
					if (p.hasGrass) grassWeight += weights[i];
					if (p.hasSand) sandWeight += weights[i];
					if (p.hasDirt) dirtWeight += weights[i];
					if (p.canHaveTrees) treeWeight += weights[i];
					if (p.canHaveWater) waterWeight += weights[i];
					if (p.treeChance > 0) avgTreeChance += (1.0f / p.treeChance) * weights[i];
				}
			}

			// Calculate terrain height with blended parameters
			float noiseVal = glm::perlin(glm::vec2(
				(worldX + randomOffset) / static_cast<float>(divisor),
				(worldZ + randomOffset) / static_cast<float>(divisor)));
			float simplexVal = glm::simplex(glm::vec2(
				(worldX + randomOffset) / static_cast<float>(divisor * 2),
				(worldZ + randomOffset) / static_cast<float>(divisor * 3)));

			int blockHeight = static_cast<int>(round(
				(noiseVal + (simplexVal + 1) / 2) * blendedAmplitude + blendedBaseHeight));

			std::vector<std::unique_ptr<Block>> vecY;
			// Underground depth - balance between cave space and performance
			const int UNDERGROUND_DEPTH = 15;
			for (int y = -UNDERGROUND_DEPTH; y < blockHeight + 5; y++)
			{
				glm::vec3 blockPos(worldX, y, worldZ);

				if (y == blockHeight)
				{
					// Check if surface should be carved as a cave entrance
					bool isCaveEntrance = ShouldCarveCave(
						static_cast<float>(worldX),
						static_cast<float>(y),
						static_cast<float>(worldZ),
						randomOffset,
						blockHeight
					);

					if (!isCaveEntrance)
					{
						// Surface block - choose based on biome weights
						if (grassWeight > sandWeight)
						{
							vecY.push_back(std::make_unique<Block>(blockPos, BlockType::GRASS));
						}
						else
						{
							vecY.push_back(std::make_unique<Block>(blockPos, BlockType::SAND));
						}
					}
					else
					{
						// Cave entrance - no surface block
						vecY.push_back(nullptr);
					}
					// Collision is added later in ApplyCollision()
				}

				if (y < blockHeight)
				{
					// Check if this position should be carved out as a cave
					bool isCave = ShouldCarveCave(
						static_cast<float>(worldX),
						static_cast<float>(y),
						static_cast<float>(worldZ),
						randomOffset,
						blockHeight
					);

					if (!isCave)
					{
						// Underground blocks - only place if not a cave
						if (dirtWeight > 0.5f && y > blockHeight - 3)
						{
							vecY.push_back(std::make_unique<Block>(blockPos, BlockType::DIRT));
						}
						else
						{
							vecY.push_back(std::make_unique<Block>(blockPos, BlockType::STONE));
						}
						// Collision is added later in ApplyCollision()
					}
					else
					{
						// Push nullptr to maintain array index alignment with Y coordinates
						// This is critical for face culling to work correctly!
						vecY.push_back(nullptr);
					}
				}

				// Water generation
				const int SEA_LEVEL = 0;
				if (waterWeight > 0.3f && y <= SEA_LEVEL && y > blockHeight)
				{
					vecY.push_back(std::make_unique<Block>(blockPos, BlockType::WATER));
				}

				// Tree generation (using deterministic hash for thread-safety)
				if (y == blockHeight + 1 && treeWeight > 0.3f && avgTreeChance > 0.0f)
				{
					// Thread-safe deterministic hash
					unsigned int hash = static_cast<unsigned int>(worldX * 73856093 ^ worldZ * 19349663);
					hash ^= hash >> 16;
					hash *= 0x85ebca6b;
					hash ^= hash >> 13;

					int effectiveTreeChance = static_cast<int>(1.0f / avgTreeChance);
					if (effectiveTreeChance > 0 && (hash % effectiveTreeChance) == 0)
					{
						Tree::Generate(blockPos, vecY);
					}
				}
			}
			vecZY.push_back(std::move(vecY));
		}
		blocksToRender.push_back(std::move(vecZY));
	}
}

void Chunk::CheckDistanceToPlayer(Player& player, Shader& shader)
{
	float distanceToPlayer = std::sqrt(
		sqr(player.position.x - ((chunkX * CHUNK_SIZE) + CHUNK_SIZE * 0.5f)) +
		sqr(player.position.z - ((chunkZ * CHUNK_SIZE) + CHUNK_SIZE * 0.5f)));

	if (distanceToPlayer > CHUNK_SIZE * 4)
	{
		isLoaded = false;
	}
	else
	{
		// Mark as loaded when ready (loading is handled by Renderer thread pool)
		if (state.load() == ChunkState::Ready)
		{
			isLoaded = true;
		}
	}
}

// Request this chunk to start loading (called from Renderer)
bool Chunk::ShouldStartLoading(const Player& player) const
{
	float distanceToPlayer = std::sqrt(
		sqr(player.position.x - ((chunkX * CHUNK_SIZE) + CHUNK_SIZE * 0.5f)) +
		sqr(player.position.z - ((chunkZ * CHUNK_SIZE) + CHUNK_SIZE * 0.5f)));

	return distanceToPlayer <= CHUNK_SIZE * 4 && state.load() == ChunkState::Unloaded;
}

bool Chunk::ShouldBuildMesh() const
{
	return state.load() == ChunkState::Generated;
}

bool Chunk::HasSolidBlockAt(int x, int z, size_t y) const
{
	if (x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
		return false;
	// Safety check: ensure blocksToRender is fully populated
	if (static_cast<size_t>(x) >= blocksToRender.size())
		return false;
	if (static_cast<size_t>(z) >= blocksToRender[x].size())
		return false;
	if (y >= blocksToRender[x][z].size())
		return false;
	auto& block = blocksToRender[x][z][y];
	if (!block)
		return false;
	// Water is NOT solid - terrain should render faces adjacent to water
	if (block->GetBlockType() == BlockType::WATER)
		return false;
	return true;
}

bool Chunk::HasBlockAt(int x, int z, size_t y) const
{
	if (x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
		return false;
	// Safety check: ensure blocksToRender is fully populated
	if (static_cast<size_t>(x) >= blocksToRender.size())
		return false;
	if (static_cast<size_t>(z) >= blocksToRender[x].size())
		return false;
	if (y >= blocksToRender[x][z].size())
		return false;
	return blocksToRender[x][z][y] != nullptr;
}

bool Chunk::RemoveBlockAtWorld(int worldX, int worldY, int worldZ)
{
	int chunkOffsetX = chunkX * CHUNK_SIZE;
	int chunkOffsetZ = chunkZ * CHUNK_SIZE;

	// Convert world coords to local chunk coords
	int localX = worldX - chunkOffsetX;
	int localZ = worldZ - chunkOffsetZ;

	// Check if this block is in this chunk
	if (localX < 0 || localX >= CHUNK_SIZE || localZ < 0 || localZ >= CHUNK_SIZE)
		return false;

	// Find the block in the Y array
	// We need to search through the Y array since blocks may be at different Y positions
	for (size_t i = 0; i < blocksToRender[localX][localZ].size(); i++)
	{
		auto& block = blocksToRender[localX][localZ][i];
		if (block && static_cast<int>(block->position.y) == worldY)
		{
			blocksToRender[localX][localZ][i].reset();
			return true;
		}
	}

	return false;
}

void Chunk::RebuildMesh()
{
	// Delete old solid buffers
	if (blocksVBO.vertexBufferId != 0)
	{
		glDeleteBuffers(1, &blocksVBO.vertexBufferId);
		blocksVBO.vertexBufferId = 0;
	}
	if (blocksIBO.indexBufferID != 0)
	{
		glDeleteBuffers(1, &blocksIBO.indexBufferID);
		blocksIBO.indexBufferID = 0;
	}

	// Delete old water buffers
	if (waterVBO.vertexBufferId != 0)
	{
		glDeleteBuffers(1, &waterVBO.vertexBufferId);
		waterVBO.vertexBufferId = 0;
	}
	if (waterIBO.indexBufferID != 0)
	{
		glDeleteBuffers(1, &waterIBO.indexBufferID);
		waterIBO.indexBufferID = 0;
	}

	// Rebuild the mesh (synchronous for now - called from main thread)
	BuildMeshData();
	UploadMeshToGPU();
}
