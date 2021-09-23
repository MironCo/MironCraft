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
	Biome biomeType;

	static const int CHUNK_SIZE = 16;
	static const int CHUNK_DEPTH = 6;

	int chunkX;
	int chunkZ;

	int randomOffset;
	int divisor;
	int amplitude;

	int totalBlocks;

	glm::mat4 model = glm::mat4(1.0f);
	VertexArray chunkVAO;
	Texture texture;
	bool hasGenerated;
public:
	bool isLoaded;
	Chunk(glm::vec2 _position, int _randomOffset, int _divisor, Shader& shader, Biome _biome)
	{
		biomeType = _biome;
		isLoaded = false;
		texture = Texture("res/textures/texture_Atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		chunkX = _position.x;
		chunkZ = _position.y;

		randomOffset = _randomOffset;
		divisor = _divisor;

		if (biomeType == Biome::OCEAN)
		{
			amplitude = 3;
		} else if (biomeType == Biome::DESERT)
		{
			amplitude = 4;
		} else if (biomeType == Biome::GRASSLAND || biomeType == Biome::FOREST)
		{
			amplitude = 4;
		} else if (biomeType == Biome::HILLS)
		{
			amplitude = 10;
		}
	}
	void DrawChunk(Shader& shader, Camera& cam)
	{
		shader.Activate();
		chunkVAO.Bind();
		texture.Bind();

		glDrawElements(GL_TRIANGLES, totalBlocks*36, GL_UNSIGNED_INT, 0);
	}
	void BatchBlocks()
	{
		std::vector<GLfloat> blockVertsVec;
		std::vector<GLuint> blockIndsVec;

		int blockNum = 0;
		
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				for (int y = 0; y < blocksToRender[x][z].size(); y++)
				{
					if (blocksToRender[x][z][y] != nullptr)
					{
						GLfloat* blockVertices = blocksToRender[x][z][y]->GetBlockVertsWithOffset(blocksToRender[x][z][y]->position, blocksToRender[x][z][y]->textureOffset, blocksToRender[x][z][y]->GetBlockType());
						GLuint* blockIndices = blocksToRender[x][z][y]->blockIndices;

						for (int v = 0; v < 192; v++)
						{
							blockVertsVec.push_back(blockVertices[v]);
						}

						for (int i = 0; i < 36; i++)
						{
							blockIndsVec.push_back((blockIndices[i] + (blockNum * 24)));
						}
						blockNum++;
						//blocksToRender[x][z][y]->Delete();
					}
				}
			}
		}

		chunkVAO.Bind();
		VertexBuffer blocksVBO(blockVertsVec.data(), blockVertsVec.size()*sizeof(float));
		IndexBuffer blocksIBO(blockIndsVec.data(), blockIndsVec.size() * sizeof(int));

		chunkVAO.Link(blocksVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		chunkVAO.Link(blocksVBO, 1, 1, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		chunkVAO.Link(blocksVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(4 * sizeof(float)));
		chunkVAO.Link(blocksVBO, 3, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		
		chunkVAO.Unbind();
		blocksVBO.Unbind();
		blocksIBO.Unbind();
		//blocksToRender.clear();
	}
	
	void Generate()
	{
		int chunkOffsetX = chunkX * CHUNK_SIZE;
		int chunkOffsetZ = chunkZ * CHUNK_SIZE;

		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			std::vector<std::vector<Block*>> vecZY;
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				int blockHeight = (int)round((glm::perlin(glm::vec2((x + randomOffset + +chunkOffsetX) / (float)(divisor), (z + randomOffset + chunkOffsetZ) / (float)(divisor)) +
					(glm::simplex(glm::vec2((x + randomOffset + chunkOffsetX) / (float)(divisor * 2), (z + randomOffset + chunkOffsetZ) / (float)(divisor * 3))) + 1) / 2)
					* (amplitude)) + biomeType);
				
				std::vector<Block*> vecY;
				for (int y = -CHUNK_DEPTH; y < blockHeight + 5; y++)
				{
					if (y == blockHeight)
					{
						if (biomeType == Biome::GRASSLAND || biomeType == Biome::HILLS || biomeType == Biome::FOREST)
						{
							Block* block = new Block(glm::vec3(x + chunkOffsetX, y, z + chunkOffsetZ), BlockType::GRASS);
							vecY.push_back(block);
						} else if (biomeType == Biome::DESERT || biomeType == Biome::OCEAN)
						{
							Block* block = new Block(glm::vec3(x + chunkOffsetX, y, z + chunkOffsetZ), BlockType::SAND);
							vecY.push_back(block);
						}
					}
					if (y < blockHeight)
					{
						if (biomeType == Biome::HILLS || biomeType == Biome::DESERT)
						{
							Block* block = new Block(glm::vec3(x + chunkOffsetX, y, z + chunkOffsetZ), BlockType::STONE);
							vecY.push_back(block);
						} else if (biomeType == Biome::GRASSLAND || biomeType == Biome::FOREST)
						{
							if (y > blockHeight - 3)
							{
								Block* block = new Block(glm::vec3(x + chunkOffsetX, y, z + chunkOffsetZ), BlockType::DIRT);
								vecY.push_back(block);
							}
							else {
								Block* block = new Block(glm::vec3(x + chunkOffsetX, y, z + chunkOffsetZ), BlockType::STONE);
								vecY.push_back(block);
							}
						}
					}
					if (biomeType != Biome::DESERT)
					{
						if (y == -1 && y > blockHeight)
						{
							Block* block = new Block(glm::vec3(x + chunkOffsetX, y, z + chunkOffsetZ), BlockType::WATER);
							vecY.push_back(block);
						}
					}
					if (y == blockHeight + 1)
					{
						if (biomeType == Biome::FOREST)
						{
							srand((x + z) * y + z);
							int treeChance = rand() % 70;
							if (treeChance == 3)
							{
								Tree* tree = new Tree(glm::vec3(x + chunkOffsetX, y, z + chunkOffsetZ), vecY);
							}
						} else if (biomeType == Biome::HILLS)
						{
							srand((x + z) * y + z);
							int treeChance = rand() % 220;
							if (treeChance == 3)
							{
								Tree* tree = new Tree(glm::vec3(x + chunkOffsetX, y, z + chunkOffsetZ), vecY);
							}
						}
					}
					totalBlocks++;
				}
				vecZY.push_back(vecY);
			}
			blocksToRender.push_back(vecZY);
		}
	}
	void CheckDistanceToCamera(Camera& _cam, Shader& shader)
	{
		float distanceToCamera = std::sqrt(sqr(_cam.position.x - ((chunkX * CHUNK_SIZE) + CHUNK_SIZE * 0.5)) + sqr(_cam.position.z - ((chunkZ * CHUNK_SIZE) + CHUNK_SIZE * 0.5)));

		if (distanceToCamera > CHUNK_SIZE * 4)
		{
			isLoaded = false;
		}
		else
		{
			isLoaded = true;
			if (hasGenerated == false)
			{
				Generate();
				BatchBlocks();
				hasGenerated = true;
			}
		}
	}
};