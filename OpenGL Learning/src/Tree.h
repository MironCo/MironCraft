#pragma once
#include "Common.h"
#include "Block.h"

class Tree
{
public:
	std::vector <Block*> treeBlocks;
public:
	Tree(glm::vec3 position, std::vector<Block*> &placeToAdd)
	{
		for (int i = 0; i < 5; i++)
		{
			Block* treeBlock = new Block(glm::vec3(position.x, position.y + i, position.z), BlockType::STONE);
			placeToAdd.push_back(treeBlock);
		}
	}
};