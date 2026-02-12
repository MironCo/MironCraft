#include "Raycast.h"
#include "Collision.h"
#include <cmath>

std::optional<RayHit> Raycast::Cast(
	const glm::vec3& origin,
	const glm::vec3& direction,
	float maxDistance)
{
	// DDA (Digital Differential Analyzer) algorithm for voxel traversal
	glm::vec3 rayDir = glm::normalize(direction);

	// Current voxel position
	glm::ivec3 mapPos(
		static_cast<int>(std::floor(origin.x)),
		static_cast<int>(std::floor(origin.y)),
		static_cast<int>(std::floor(origin.z))
	);

	// Length of ray from one voxel boundary to the next
	glm::vec3 deltaDist(
		std::abs(1.0f / rayDir.x),
		std::abs(1.0f / rayDir.y),
		std::abs(1.0f / rayDir.z)
	);

	// Step direction (+1 or -1)
	glm::ivec3 step(
		rayDir.x >= 0 ? 1 : -1,
		rayDir.y >= 0 ? 1 : -1,
		rayDir.z >= 0 ? 1 : -1
	);

	// Distance to next voxel boundary
	glm::vec3 sideDist;
	if (rayDir.x < 0)
		sideDist.x = (origin.x - mapPos.x) * deltaDist.x;
	else
		sideDist.x = (mapPos.x + 1.0f - origin.x) * deltaDist.x;

	if (rayDir.y < 0)
		sideDist.y = (origin.y - mapPos.y) * deltaDist.y;
	else
		sideDist.y = (mapPos.y + 1.0f - origin.y) * deltaDist.y;

	if (rayDir.z < 0)
		sideDist.z = (origin.z - mapPos.z) * deltaDist.z;
	else
		sideDist.z = (mapPos.z + 1.0f - origin.z) * deltaDist.z;

	// Track the last axis we stepped in for normal calculation
	int lastAxis = -1;
	float distance = 0.0f;

	// Step through the grid
	while (distance < maxDistance)
	{
		// Determine which axis to step in (smallest sideDist)
		if (sideDist.x < sideDist.y && sideDist.x < sideDist.z)
		{
			distance = sideDist.x;
			sideDist.x += deltaDist.x;
			mapPos.x += step.x;
			lastAxis = 0;
		}
		else if (sideDist.y < sideDist.z)
		{
			distance = sideDist.y;
			sideDist.y += deltaDist.y;
			mapPos.y += step.y;
			lastAxis = 1;
		}
		else
		{
			distance = sideDist.z;
			sideDist.z += deltaDist.z;
			mapPos.z += step.z;
			lastAxis = 2;
		}

		// Check if there's a block at current position (after stepping)
		if (g_CollisionWorld.HasBlock(mapPos.x, mapPos.y, mapPos.z))
		{
			RayHit hit;
			hit.blockPos = mapPos;
			hit.distance = distance;

			// Set normal based on which face we hit
			hit.normal = glm::ivec3(0);
			if (lastAxis == 0)
				hit.normal.x = -step.x;
			else if (lastAxis == 1)
				hit.normal.y = -step.y;
			else if (lastAxis == 2)
				hit.normal.z = -step.z;

			return hit;
		}
	}

	return std::nullopt;
}
