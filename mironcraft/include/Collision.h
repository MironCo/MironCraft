#pragma once
#include <GLM/glm.hpp>
#include <unordered_set>
#include <functional>
#include "Player.h"

struct BlockPos
{
	int x, y, z;

	bool operator==(const BlockPos& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}
};

struct BlockPosHash
{
	std::size_t operator()(const BlockPos& pos) const
	{
		return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1) ^ (std::hash<int>()(pos.z) << 2);
	}
};

class CollisionWorld
{
private:
	std::unordered_set<BlockPos, BlockPosHash> solidBlocks;

public:
	void AddBlock(int x, int y, int z);
	void RemoveBlock(int x, int y, int z);
	bool HasBlock(int x, int y, int z) const;

	AABB GetBlockAABB(int x, int y, int z) const;
	bool CheckCollision(const AABB& playerAABB) const;
	glm::vec3 GetDepenetration(const AABB& playerAABB) const;
	void ResolveCollision(Player& player, float deltaTime);
};

extern CollisionWorld g_CollisionWorld;
