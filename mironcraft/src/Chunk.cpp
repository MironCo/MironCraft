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

void Chunk::BatchBlocks()
{
	std::vector<GLfloat> blockVertsVec;
	std::vector<GLuint> blockIndsVec;

	int currentVertex = 0;

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

					// Check each face for visibility (only render if no neighbor)
					// Face order: Front(+Z), Back(-Z), Left(-X), Right(+X), Top(+Y), Bottom(-Y)
					bool faces[6];
					faces[0] = !HasBlockAt(x, z + 1, y);     // Front (+Z)
					faces[1] = !HasBlockAt(x, z - 1, y);     // Back (-Z)
					faces[2] = !HasBlockAt(x - 1, z, y);     // Left (-X)
					faces[3] = !HasBlockAt(x + 1, z, y);     // Right (+X)
					faces[4] = !HasBlockAt(x, z, y + 1);     // Top (+Y)
					faces[5] = (y == 0) || !HasBlockAt(x, z, y - 1); // Bottom (-Y)

					for (int face = 0; face < 6; face++)
					{
						if (faces[face])
						{
							// Each face has 4 vertices × 8 floats = 32 floats
							int vertStart = face * 32;
							for (int v = 0; v < 32; v++)
							{
								blockVertsVec.push_back(blockVertices[vertStart + v]);
							}

							// Index patterns vary by face (from Block.h blockIndices)
							// Front/Back/Left: 0,1,2, 0,2,3
							// Right/Top/Bottom: different patterns
							if (face < 3) // Front, Back, Left
							{
								blockIndsVec.push_back(currentVertex + 0);
								blockIndsVec.push_back(currentVertex + 1);
								blockIndsVec.push_back(currentVertex + 2);
								blockIndsVec.push_back(currentVertex + 0);
								blockIndsVec.push_back(currentVertex + 2);
								blockIndsVec.push_back(currentVertex + 3);
							}
							else if (face == 3) // Right: 12,13,14, 14,15,13
							{
								blockIndsVec.push_back(currentVertex + 0);
								blockIndsVec.push_back(currentVertex + 1);
								blockIndsVec.push_back(currentVertex + 2);
								blockIndsVec.push_back(currentVertex + 2);
								blockIndsVec.push_back(currentVertex + 3);
								blockIndsVec.push_back(currentVertex + 1);
							}
							else // Top/Bottom: 16,17,18, 18,19,17 and 20,21,22, 22,23,21
							{
								blockIndsVec.push_back(currentVertex + 0);
								blockIndsVec.push_back(currentVertex + 1);
								blockIndsVec.push_back(currentVertex + 2);
								blockIndsVec.push_back(currentVertex + 2);
								blockIndsVec.push_back(currentVertex + 3);
								blockIndsVec.push_back(currentVertex + 1);
							}

							currentVertex += 4;
						}
					}
				}
			}
		}
	}

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
					if (biomeType == Biome::HILLS || biomeType == Biome::DESERT)
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

				if (biomeType != Biome::DESERT && y == -1 && y > blockHeight)
				{
					vecY.push_back(std::make_unique<Block>(blockPos, BlockType::WATER));
					// Water is not solid, no collision
				}

				if (y == blockHeight + 1)
				{
					srand((x + z) * y + z);
					int treeChance = rand() % (biomeType == Biome::FOREST ? 70 : 220);
					if ((biomeType == Biome::FOREST || biomeType == Biome::HILLS) && treeChance == 3)
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

bool Chunk::HasBlockAt(int x, int z, size_t y) const
{
	if (x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
		return false;
	if (y >= blocksToRender[x][z].size())
		return false;
	return blocksToRender[x][z][y] != nullptr;
}
