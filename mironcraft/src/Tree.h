#pragma once
#include "Common.h"
#include "Block.h"

class Tree
{
public:
	Tree(glm::vec3 position, std::vector<Block*>& placeToAdd)
	{
		srand(position.x);
		int treeHeight = rand() % 4 + 3;

		//Generate Trunk
		for (int i = 0; i < treeHeight; i++)
		{
			Block* treeBlock = new Block(glm::vec3(position.x, position.y + i, position.z), BlockType::LOG);
			placeToAdd.push_back(treeBlock);
		}
		
		//Generate Leaves
		for (int x = -2; x < 3; x++)
		{
			for (int z = -2; z < 3; z++)
			{
				for (int y = -1; y < 1; y++)
				{
					if (z != 0 || x != 0)
					{
						if ((x == -2 && z == -2) || (x == 2 && z == -2) || (x == -2 && z == 2) || (x == 2 && z == 2))
						{
							continue;
						}
						else
						{
							Block* treeBlock = new Block(glm::vec3(position.x + x, position.y + (treeHeight - 1) + y, position.z + z), BlockType::LEAVES);
							placeToAdd.push_back(treeBlock);
						}
					}
				}
			}
		}
		//Final Leaf On Top
		Block* treeBlock = new Block(glm::vec3(position.x, position.y + treeHeight, position.z), BlockType::LEAVES);
		placeToAdd.push_back(treeBlock);
	}
};