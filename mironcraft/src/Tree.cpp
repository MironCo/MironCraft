#include "Tree.h"
#include "Collision.h"
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
			glm::vec3 logPos(position.x, position.y + i, position.z);
			blocks.push_back(std::make_unique<Block>(logPos, BlockType::LOG));
			g_CollisionWorld.AddBlock(logPos.x, logPos.y, logPos.z);
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
							glm::vec3 leafPos(position.x + x, position.y + (treeHeight - 1) + y, position.z + z);
							blocks.push_back(std::make_unique<Block>(leafPos, BlockType::LEAVES));
							g_CollisionWorld.AddBlock(leafPos.x, leafPos.y, leafPos.z);
						}
					}
				}
			}
		}

		// Final Leaf On Top
		glm::vec3 topLeafPos(position.x, position.y + treeHeight, position.z);
		blocks.push_back(std::make_unique<Block>(topLeafPos, BlockType::LEAVES));
		g_CollisionWorld.AddBlock(topLeafPos.x, topLeafPos.y, topLeafPos.z);
	}
}
