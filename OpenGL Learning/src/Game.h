#pragma once

#include <cmath>
#include <GLM/gtc/noise.hpp>

#include "Chunk.h"
#include "Renderer.h";

class Game
{
private:
	static const int worldSize = 100;
	static const int worldAmplitude = 3;
	static const int divisor = 15;
public:
	static const int worldCenter = (worldSize * 16)/2;
public:
	static inline void Start(Shader &_shader)
	{
		GenerateWorld(_shader);
	}
	static inline void GenerateWorld(Shader &_shader)
	{
		srand(time(0));
		unsigned int randomOffset = rand() % UINT32_MAX;
	
		for (int x = 0; x < worldSize; x++)
		{
			for (int z = 0; z < worldSize; z++)
			{
				float biomeType = glm::simplex(glm::vec2(x/4 + randomOffset, z/4 + randomOffset));

				//std::cout << biomeType << std::endl;

				if (biomeType >= -1 && biomeType < -0.3f)
				{
					Chunk* chunk = new Chunk(glm::vec2(x, z), randomOffset, divisor, _shader, Biome::OCEAN);
					Renderer::AddToRender(chunk);
					continue;
				} else if (biomeType >= -0.3f && biomeType < 0.2f)
				{
					Chunk* chunk = new Chunk(glm::vec2(x, z), randomOffset, divisor, _shader, Biome::DESERT);
					Renderer::AddToRender(chunk);
					continue;
				} else if (biomeType >= 0.2f && biomeType < 0.6f)
				{
					Chunk* chunk = new Chunk(glm::vec2(x, z), randomOffset, divisor, _shader, Biome::GRASSLAND);
					Renderer::AddToRender(chunk);
					continue;
				} else if (biomeType >= 0.6f && biomeType <= 1.0f)
				{
					Chunk* chunk = new Chunk(glm::vec2(x, z), randomOffset, divisor, _shader, Biome::HILLS);
					Renderer::AddToRender(chunk);
					continue;
				}
			}
		}
	}
};