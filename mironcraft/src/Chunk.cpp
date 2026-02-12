#include "Chunk.h"
#include "Tree.h"
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

void Chunk::DrawChunk(Shader& shader, Camera& cam)
{
	shader.Activate();
	chunkVAO.Bind();
	texture.Bind();
	glDrawElements(GL_TRIANGLES, totalBlocks * 36, GL_UNSIGNED_INT, 0);
}

void Chunk::BatchBlocks()
{
	std::vector<GLfloat> blockVertsVec;
	std::vector<GLuint> blockIndsVec;

	int blockNum = 0;

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
					GLuint* blockIndices = block->blockIndices;

					for (int v = 0; v < 192; v++)
					{
						blockVertsVec.push_back(blockVertices[v]);
					}

					for (int i = 0; i < 36; i++)
					{
						blockIndsVec.push_back(blockIndices[i] + (blockNum * 24));
					}
					blockNum++;
				}
			}
		}
	}

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
					}
					else if (biomeType == Biome::DESERT || biomeType == Biome::OCEAN)
					{
						vecY.push_back(std::make_unique<Block>(blockPos, BlockType::SAND));
					}
				}

				if (y < blockHeight)
				{
					if (biomeType == Biome::HILLS || biomeType == Biome::DESERT)
					{
						vecY.push_back(std::make_unique<Block>(blockPos, BlockType::STONE));
					}
					else if (biomeType == Biome::GRASSLAND || biomeType == Biome::FOREST)
					{
						if (y > blockHeight - 3)
						{
							vecY.push_back(std::make_unique<Block>(blockPos, BlockType::DIRT));
						}
						else
						{
							vecY.push_back(std::make_unique<Block>(blockPos, BlockType::STONE));
						}
					}
				}

				if (biomeType != Biome::DESERT && y == -1 && y > blockHeight)
				{
					vecY.push_back(std::make_unique<Block>(blockPos, BlockType::WATER));
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

				totalBlocks++;
			}
			vecZY.push_back(std::move(vecY));
		}
		blocksToRender.push_back(std::move(vecZY));
	}
}

void Chunk::CheckDistanceToCamera(Camera& cam, Shader& shader)
{
	float distanceToCamera = std::sqrt(
		sqr(cam.position.x - ((chunkX * CHUNK_SIZE) + CHUNK_SIZE * 0.5f)) +
		sqr(cam.position.z - ((chunkZ * CHUNK_SIZE) + CHUNK_SIZE * 0.5f)));

	if (distanceToCamera > CHUNK_SIZE * 4)
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
