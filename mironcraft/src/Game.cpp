#include "Game.h"
#include "Chunk.h"
#include "Renderer.h"

#include <ctime>
#include <cstdlib>
#include <GLM/glm.hpp>

namespace Game
{
	static const int worldSize = 100;
	static const int divisor = 15;
	const int worldCenter = (worldSize * 16) / 2;

	void Start(Shader& shader)
	{
		srand(time(0));
		int randomOffset = rand() % 10000;

		for (int x = 0; x < worldSize; x++)
		{
			for (int z = 0; z < worldSize; z++)
			{
				auto chunk = std::make_unique<Chunk>(glm::vec2(x, z), randomOffset, divisor, shader, Biome::FOREST);
				Renderer::AddToRender(std::move(chunk));
			}
		}
	}
}
