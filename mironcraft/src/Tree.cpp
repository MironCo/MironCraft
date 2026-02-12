#include "Tree.h"
#include <cstdlib>

namespace Tree
{
	void Generate(glm::vec3 position, std::vector<std::unique_ptr<Block>>& blocks)
	{
		srand(static_cast<unsigned int>(position.x));
		int treeHeight = rand() % 4 + 3;

		// Generate Trunk
		for (int i = 0; i < treeHeight; i++)
		{
			blocks.push_back(std::make_unique<Block>(
				glm::vec3(position.x, position.y + i, position.z), BlockType::LOG));
		}

		// Generate Leaves
		for (int x = -2; x < 3; x++)
		{
			for (int z = -2; z < 3; z++)
			{
				for (int y = -1; y < 1; y++)
				{
					if (z != 0 || x != 0)
					{
						// Skip corners
						bool isCorner = (x == -2 && z == -2) || (x == 2 && z == -2) ||
						                (x == -2 && z == 2) || (x == 2 && z == 2);
						if (!isCorner)
						{
							blocks.push_back(std::make_unique<Block>(
								glm::vec3(position.x + x, position.y + (treeHeight - 1) + y, position.z + z),
								BlockType::LEAVES));
						}
					}
				}
			}
		}

		// Final Leaf On Top
		blocks.push_back(std::make_unique<Block>(
			glm::vec3(position.x, position.y + treeHeight, position.z), BlockType::LEAVES));
	}
}
