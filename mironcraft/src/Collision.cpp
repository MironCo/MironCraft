#include "Collision.h"
#include <cmath>

CollisionWorld g_CollisionWorld;

void CollisionWorld::AddBlock(int x, int y, int z)
{
	solidBlocks.insert({x, y, z});
}

void CollisionWorld::RemoveBlock(int x, int y, int z)
{
	solidBlocks.erase({x, y, z});
}

bool CollisionWorld::HasBlock(int x, int y, int z) const
{
	return solidBlocks.find({x, y, z}) != solidBlocks.end();
}

AABB CollisionWorld::GetBlockAABB(int x, int y, int z) const
{
	return AABB(
		glm::vec3(x, y, z),
		glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f)
	);
}

bool CollisionWorld::CheckCollision(const AABB& playerAABB) const
{
	int minX = static_cast<int>(std::floor(playerAABB.min.x));
	int minY = static_cast<int>(std::floor(playerAABB.min.y));
	int minZ = static_cast<int>(std::floor(playerAABB.min.z));
	int maxX = static_cast<int>(std::floor(playerAABB.max.x));
	int maxY = static_cast<int>(std::floor(playerAABB.max.y));
	int maxZ = static_cast<int>(std::floor(playerAABB.max.z));

	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			for (int z = minZ; z <= maxZ; z++)
			{
				if (HasBlock(x, y, z))
				{
					AABB blockAABB = GetBlockAABB(x, y, z);
					if (playerAABB.Intersects(blockAABB))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

void CollisionWorld::ResolveCollision(Player& player, float deltaTime)
{
	const float epsilon = 0.001f;

	// Try X movement
	float moveX = player.velocity.x * deltaTime;
	if (std::abs(moveX) > epsilon)
	{
		glm::vec3 newPos = player.position;
		newPos.x += moveX;
		Player testPlayer = player;
		testPlayer.position = newPos;
		if (CheckCollision(testPlayer.GetAABB()))
		{
			player.velocity.x = 0;
		}
		else
		{
			player.position.x = newPos.x;
		}
	}

	// Try Z movement
	float moveZ = player.velocity.z * deltaTime;
	if (std::abs(moveZ) > epsilon)
	{
		glm::vec3 newPos = player.position;
		newPos.z += moveZ;
		Player testPlayer = player;
		testPlayer.position = newPos;
		if (CheckCollision(testPlayer.GetAABB()))
		{
			player.velocity.z = 0;
		}
		else
		{
			player.position.z = newPos.z;
		}
	}

	// Try Y movement
	float moveY = player.velocity.y * deltaTime;
	glm::vec3 newPosY = player.position;
	newPosY.y += moveY;
	Player testPlayerY = player;
	testPlayerY.position = newPosY;
	if (CheckCollision(testPlayerY.GetAABB()))
	{
		if (player.velocity.y <= 0)
		{
			player.isGrounded = true;
		}
		player.velocity.y = 0;
	}
	else
	{
		player.position.y = newPosY.y;
		// Check if we're still grounded (for walking down slopes)
		Player groundCheck = player;
		groundCheck.position.y -= 0.05f;
		player.isGrounded = CheckCollision(groundCheck.GetAABB());
	}
}
