#pragma once
#include <GLM/glm.hpp>
#include <optional>

struct RayHit
{
	glm::ivec3 blockPos;   // The block that was hit
	glm::ivec3 normal;     // The face normal (for placing blocks)
	float distance;
};

class Raycast
{
public:
	// Cast a ray and find the first block hit
	// Returns nullopt if no block hit within maxDistance
	static std::optional<RayHit> Cast(
		const glm::vec3& origin,
		const glm::vec3& direction,
		float maxDistance = 5.0f
	);
};
