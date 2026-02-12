#include "Tree.h"

// Thread-safe hash function for deterministic tree height
static unsigned int HashPosition(int x, int z)
{
	unsigned int h = static_cast<unsigned int>(x * 73856093 ^ z * 19349663);
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

namespace Tree
{
	void Generate(glm::vec3 position, std::vector<std::unique_ptr<Block>>& blocks)
	{
		// Use deterministic hash instead of srand/rand (thread-safe)
		unsigned int hash = HashPosition(static_cast<int>(position.x), static_cast<int>(position.z));
		int treeHeight = (hash % 4) + 3;

		// Generate Trunk
		for (int i = 0; i < treeHeight; i++)
		{
			glm::vec3 logPos(position.x, position.y + i, position.z);
			blocks.push_back(std::make_unique<Block>(logPos, BlockType::LOG));
			// Collision is added later in Chunk::ApplyCollision()
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
							// Collision is added later in Chunk::ApplyCollision()
						}
					}
				}
			}
		}

		// Final Leaf On Top
		glm::vec3 topLeafPos(position.x, position.y + treeHeight, position.z);
		blocks.push_back(std::make_unique<Block>(topLeafPos, BlockType::LEAVES));
		// Collision is added later in Chunk::ApplyCollision()
	}
}
