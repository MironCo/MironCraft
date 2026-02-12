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
		int randomOffset = rand() % 10000;

		for (int x = 0; x < worldSize; x++)
		{
			for (int z = 0; z < worldSize; z++)
			{
				Chunk* chunk = new Chunk(glm::vec2(x, z), randomOffset, divisor, _shader, Biome::FOREST);
				Renderer::AddToRender(chunk);
			}
		}
	}
};