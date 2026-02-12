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

void Chunk::BatchBlocks()
{
	std::vector<GLfloat> blockVertsVec;
	std::vector<GLuint> blockIndsVec;
	std::vector<GLfloat> waterVertsVec;
	std::vector<GLuint> waterIndsVec;

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
								AddFace(face, blockVertices, currentWaterVertex, waterVertsVec, waterIndsVec);
							}
							else
							{
								AddFace(face, blockVertices, currentVertex, blockVertsVec, blockIndsVec);
							}
						}
					}
				}
			}
		}
	}

	// Setup solid block mesh
	totalIndices = static_cast<int>(blockIndsVec.size());

	chunkVAO.Bind();

	glGenBuffers(1, &blocksVBO.vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, blocksVBO.vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, blockVertsVec.size() * sizeof(float), blockVertsVec.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &blocksIBO.indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blocksIBO.indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, blockIndsVec.size() * sizeof(int), blockIndsVec.data(), GL_STATIC_DRAW);

	chunkVAO.Link(blocksVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	chunkVAO.Link(blocksVBO, 1, 1, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	chunkVAO.Link(blocksVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(4 * sizeof(float)));
	chunkVAO.Link(blocksVBO, 3, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	chunkVAO.Unbind();
	blocksVBO.Unbind();
	blocksIBO.Unbind();

	// Setup water mesh
	totalWaterIndices = static_cast<int>(waterIndsVec.size());

	if (totalWaterIndices > 0)
	{
		waterVAO.Bind();

		glGenBuffers(1, &waterVBO.vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBO.vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, waterVertsVec.size() * sizeof(float), waterVertsVec.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &waterIBO.indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterIBO.indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterIndsVec.size() * sizeof(int), waterIndsVec.data(), GL_STATIC_DRAW);

		waterVAO.Link(waterVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		waterVAO.Link(waterVBO, 1, 1, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		waterVAO.Link(waterVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(4 * sizeof(float)));
		waterVAO.Link(waterVBO, 3, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		waterVAO.Unbind();
		waterVBO.Unbind();
		waterIBO.Unbind();
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
			float noiseVal = glm::perlin(glm::vec2(
				(x + randomOffset + chunkOffsetX) / static_cast<float>(divisor),
				(z + randomOffset + chunkOffsetZ) / static_cast<float>(divisor)));
			float simplexVal = glm::simplex(glm::vec2(
				(x + randomOffset + chunkOffsetX) / static_cast<float>(divisor * 2),
				(z + randomOffset + chunkOffsetZ) / static_cast<float>(divisor * 3)));

			int blockHeight = static_cast<int>(round((noiseVal + (simplexVal + 1) / 2) * amplitude)) + biomeType;

			std::vector<std::unique_ptr<Block>> vecY;
			for (int y = -CHUNK_DEPTH; y < blockHeight + 5; y++)
			{
				glm::vec3 blockPos(x + chunkOffsetX, y, z + chunkOffsetZ);

				if (y == blockHeight)
				{
					if (biomeType == Biome::GRASSLAND || biomeType == Biome::HILLS || biomeType == Biome::FOREST)
					{
						vecY.push_back(std::make_unique<Block>(blockPos, BlockType::GRASS));
						g_CollisionWorld.AddBlock(blockPos.x, blockPos.y, blockPos.z);
					}
					else if (biomeType == Biome::DESERT || biomeType == Biome::OCEAN)
					{
						vecY.push_back(std::make_unique<Block>(blockPos, BlockType::SAND));
						g_CollisionWorld.AddBlock(blockPos.x, blockPos.y, blockPos.z);
					}
				}

				if (y < blockHeight)
				{
					if (biomeType == Biome::HILLS || biomeType == Biome::DESERT || biomeType == Biome::OCEAN)
					{
						vecY.push_back(std::make_unique<Block>(blockPos, BlockType::STONE));
						g_CollisionWorld.AddBlock(blockPos.x, blockPos.y, blockPos.z);
					}
					else if (biomeType == Biome::GRASSLAND || biomeType == Biome::FOREST)
					{
						if (y > blockHeight - 3)
						{
							vecY.push_back(std::make_unique<Block>(blockPos, BlockType::DIRT));
							g_CollisionWorld.AddBlock(blockPos.x, blockPos.y, blockPos.z);
						}
						else
						{
							vecY.push_back(std::make_unique<Block>(blockPos, BlockType::STONE));
							g_CollisionWorld.AddBlock(blockPos.x, blockPos.y, blockPos.z);
						}
					}
				}

				// Fill water from sea level (y=0) down to terrain surface
				// Water appears where terrain is below sea level
				const int SEA_LEVEL = 0;
				if (biomeType != Biome::DESERT && y <= SEA_LEVEL && y > blockHeight)
				{
					vecY.push_back(std::make_unique<Block>(blockPos, BlockType::WATER));
					// Water is not solid, no collision
				}

				if (y == blockHeight + 1)
				{
					// Use world position for deterministic but varied seeding
					srand(static_cast<unsigned int>((x + chunkOffsetX) * 73856093 ^ (z + chunkOffsetZ) * 19349663));
					int treeChance = rand() % (biomeType == Biome::FOREST ? 70 : 220);
					if ((biomeType == Biome::FOREST || biomeType == Biome::HILLS) && treeChance == 0)
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
		isLoaded = true;
		if (!hasGenerated)
		{
			Generate();
			BatchBlocks();
			hasGenerated = true;
		}
	}
}

bool Chunk::HasSolidBlockAt(int x, int z, size_t y) const
{
	if (x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
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

	// Rebuild the mesh
	BatchBlocks();
}
