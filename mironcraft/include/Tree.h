#pragma once
#include "Block.h"
#include <memory>
#include <vector>

namespace Tree
{
	void Generate(glm::vec3 position, std::vector<std::unique_ptr<Block>>& blocks);
}
