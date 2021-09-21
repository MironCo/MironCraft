#pragma once
#include "Common.h"
#include "Block.h"

class Tree
{
public:
	Tree(glm::vec3 position, std::vector<Block*>& placeToAdd)
	{
		srand(position.x);
		int treeHeight = rand() % 3 + 3;

		for (int i = 0; i < treeHeight; i++)
		{
			Block* treeBlock = new Block(glm::vec3(position.x, position.y + i, position.z), BlockType::LOG);
			placeToAdd.push_back(treeBlock);
		}

		for (int x = -2; x < 3; x++)
		{
			for (int z = -2; z < 3; z++)
			{
				if (z != 0 || x != 0)
				{
					if (x != -2 || x != 2)
					{
						Block* treeBlock = new Block(glm::vec3(position.x + x, position.y + (treeHeight - 1), position.z + z), BlockType::LEAVES);
						placeToAdd.push_back(treeBlock);
					}
				}
			}
		}
	}
};