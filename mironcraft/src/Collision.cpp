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
	// Block visuals are centered at (x,y,z), spanning -0.5 to +0.5
	return AABB(
		glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f),
		glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f)
	);
}

bool CollisionWorld::CheckCollision(const AABB& playerAABB) const
{
	// Blocks are centered at integer coords (span -0.5 to +0.5 around integer)
	// Expand range by 1 to catch all edge cases
	int minX = static_cast<int>(std::floor(playerAABB.min.x + 0.5f)) - 1;
	int minY = static_cast<int>(std::floor(playerAABB.min.y + 0.5f)) - 1;
	int minZ = static_cast<int>(std::floor(playerAABB.min.z + 0.5f)) - 1;
	int maxX = static_cast<int>(std::floor(playerAABB.max.x + 0.5f)) + 1;
	int maxY = static_cast<int>(std::floor(playerAABB.max.y + 0.5f)) + 1;
	int maxZ = static_cast<int>(std::floor(playerAABB.max.z + 0.5f)) + 1;

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

// Calculate penetration depth and push player out of blocks
glm::vec3 CollisionWorld::GetDepenetration(const AABB& playerAABB) const
{
	glm::vec3 totalPush(0.0f);

	// Blocks are centered at integer coords (span -0.5 to +0.5 around integer)
	// Expand range by 1 to catch all edge cases
	int minX = static_cast<int>(std::floor(playerAABB.min.x + 0.5f)) - 1;
	int minY = static_cast<int>(std::floor(playerAABB.min.y + 0.5f)) - 1;
	int minZ = static_cast<int>(std::floor(playerAABB.min.z + 0.5f)) - 1;
	int maxX = static_cast<int>(std::floor(playerAABB.max.x + 0.5f)) + 1;
	int maxY = static_cast<int>(std::floor(playerAABB.max.y + 0.5f)) + 1;
	int maxZ = static_cast<int>(std::floor(playerAABB.max.z + 0.5f)) + 1;

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
						// Calculate overlap on each axis
						float overlapX1 = blockAABB.max.x - playerAABB.min.x;
						float overlapX2 = playerAABB.max.x - blockAABB.min.x;
						float overlapY1 = blockAABB.max.y - playerAABB.min.y;
						float overlapY2 = playerAABB.max.y - blockAABB.min.y;
						float overlapZ1 = blockAABB.max.z - playerAABB.min.z;
						float overlapZ2 = playerAABB.max.z - blockAABB.min.z;

						// Find minimum overlap direction
						float minOverlapX = (overlapX1 < overlapX2) ? overlapX1 : -overlapX2;
						float minOverlapY = (overlapY1 < overlapY2) ? overlapY1 : -overlapY2;
						float minOverlapZ = (overlapZ1 < overlapZ2) ? overlapZ1 : -overlapZ2;

						// Push out along axis with smallest overlap
						if (std::abs(minOverlapX) <= std::abs(minOverlapY) && std::abs(minOverlapX) <= std::abs(minOverlapZ))
						{
							totalPush.x += minOverlapX;
						}
						else if (std::abs(minOverlapZ) <= std::abs(minOverlapY))
						{
							totalPush.z += minOverlapZ;
						}
						else
						{
							totalPush.y += minOverlapY;
						}
					}
				}
			}
		}
	}

	return totalPush;
}

void CollisionWorld::ResolveCollision(Player& player, float deltaTime)
{
	const float epsilon = 0.001f;
	const float skinWidth = 0.01f;
	const int maxIterations = 8;

	// First: depenetrate if already inside blocks
	for (int i = 0; i < maxIterations; i++)
	{
		AABB playerAABB = player.GetAABB();
		glm::vec3 push(0.0f);
		float smallestPush = 1000.0f;
		int pushAxis = -1;
		float pushDir = 0.0f;

		// Blocks are centered at integer coords (span -0.5 to +0.5 around integer)
		// Use round to find the closest block center, then check ±1 to be safe
		int minX = static_cast<int>(std::floor(playerAABB.min.x + 0.5f)) - 1;
		int minY = static_cast<int>(std::floor(playerAABB.min.y + 0.5f)) - 1;
		int minZ = static_cast<int>(std::floor(playerAABB.min.z + 0.5f)) - 1;
		int maxX = static_cast<int>(std::floor(playerAABB.max.x + 0.5f)) + 1;
		int maxY = static_cast<int>(std::floor(playerAABB.max.y + 0.5f)) + 1;
		int maxZ = static_cast<int>(std::floor(playerAABB.max.z + 0.5f)) + 1;

		for (int x = minX; x <= maxX; x++)
		{
			for (int y = minY; y <= maxY; y++)
			{
				for (int z = minZ; z <= maxZ; z++)
				{
					if (!HasBlock(x, y, z)) continue;

					AABB blockAABB = GetBlockAABB(x, y, z);
					if (!playerAABB.Intersects(blockAABB)) continue;

					// Calculate overlap on each axis
					float overlapPosX = blockAABB.max.x - playerAABB.min.x;
					float overlapNegX = playerAABB.max.x - blockAABB.min.x;
					float overlapPosY = blockAABB.max.y - playerAABB.min.y;
					float overlapNegY = playerAABB.max.y - blockAABB.min.y;
					float overlapPosZ = blockAABB.max.z - playerAABB.min.z;
					float overlapNegZ = playerAABB.max.z - blockAABB.min.z;

					// Find smallest push direction
					if (overlapPosX < smallestPush) { smallestPush = overlapPosX; pushAxis = 0; pushDir = 1.0f; }
					if (overlapNegX < smallestPush) { smallestPush = overlapNegX; pushAxis = 0; pushDir = -1.0f; }
					if (overlapPosY < smallestPush) { smallestPush = overlapPosY; pushAxis = 1; pushDir = 1.0f; }
					if (overlapNegY < smallestPush) { smallestPush = overlapNegY; pushAxis = 1; pushDir = -1.0f; }
					if (overlapPosZ < smallestPush) { smallestPush = overlapPosZ; pushAxis = 2; pushDir = 1.0f; }
					if (overlapNegZ < smallestPush) { smallestPush = overlapNegZ; pushAxis = 2; pushDir = -1.0f; }
				}
			}
		}

		if (pushAxis < 0 || smallestPush < epsilon)
		{
			break;
		}

		// Apply push with skin width
		float pushAmount = (smallestPush + skinWidth) * pushDir;
		if (pushAxis == 0) { player.position.x += pushAmount; player.velocity.x = 0; }
		else if (pushAxis == 1) { player.position.y += pushAmount; player.velocity.y = 0; if (pushDir > 0) player.isGrounded = true; }
		else if (pushAxis == 2) { player.position.z += pushAmount; player.velocity.z = 0; }
	}

	// Second: apply velocity with collision checking
	glm::vec3 movement = player.velocity * deltaTime;

	// X axis
	if (std::abs(movement.x) > epsilon)
	{
		player.position.x += movement.x;
		if (CheckCollision(player.GetAABB()))
		{
			player.position.x -= movement.x;
			player.velocity.x = 0;
		}
	}

	// Z axis
	if (std::abs(movement.z) > epsilon)
	{
		player.position.z += movement.z;
		if (CheckCollision(player.GetAABB()))
		{
			player.position.z -= movement.z;
			player.velocity.z = 0;
		}
	}

	// Y axis
	if (std::abs(movement.y) > epsilon)
	{
		player.position.y += movement.y;
		if (CheckCollision(player.GetAABB()))
		{
			player.position.y -= movement.y;
			if (player.velocity.y <= 0)
			{
				player.isGrounded = true;
			}
			player.velocity.y = 0;
		}
		else
		{
			// Check if still grounded
			Player groundCheck = player;
			groundCheck.position.y -= 0.05f;
			player.isGrounded = CheckCollision(groundCheck.GetAABB());
		}
	}
	else
	{
		// Even with no Y movement, check if we're still grounded (walking off ledges)
		Player groundCheck = player;
		groundCheck.position.y -= 0.05f;
		player.isGrounded = CheckCollision(groundCheck.GetAABB());
	}
}
