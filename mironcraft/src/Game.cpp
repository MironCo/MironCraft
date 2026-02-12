#include "Game.h"
#include "Chunk.h"
#include "Renderer.h"

#include <ctime>
#include <cstdlib>
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>

namespace Game
{
	static const int worldSize = 100;
	static const int divisor = 15;
	const int worldCenter = (worldSize * 16) / 2;

	// Determine biome based on chunk position using noise
	Biome GetBiomeAt(int chunkX, int chunkZ, int seed)
	{
		// Use large-scale noise for biome regions
		float biomeScale = 0.05f; // Larger = smaller biomes
		float moisture = glm::simplex(glm::vec2(
			(chunkX + seed) * biomeScale,
			(chunkZ + seed) * biomeScale
		));
		float temperature = glm::simplex(glm::vec2(
			(chunkX + seed + 1000) * biomeScale,
			(chunkZ + seed + 1000) * biomeScale
		));

		// Map noise values (-1 to 1) to biomes
		// Low moisture + high temp = desert
		// Low temperature/moisture = ocean
		// High moisture = forest
		// Medium = grassland or hills

		if (temperature > 0.3f && moisture < -0.2f)
			return Biome::DESERT;
		if (temperature < -0.4f || moisture < -0.5f)
			return Biome::OCEAN;
		if (moisture > 0.3f)
			return Biome::FOREST;
		if (temperature > 0.0f && moisture > -0.2f)
			return Biome::HILLS;

		return Biome::GRASSLAND;
	}

	void Start(Shader& shader)
	{
		srand(time(0));
		int randomOffset = rand() % 10000;

		for (int x = 0; x < worldSize; x++)
		{
			for (int z = 0; z < worldSize; z++)
			{
				Biome biome = GetBiomeAt(x, z, randomOffset);
				auto chunk = std::make_unique<Chunk>(glm::vec2(x, z), randomOffset, divisor, shader, biome);
				Renderer::AddToRender(std::move(chunk));
			}
		}
	}
}
