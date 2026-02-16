#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/rotate_vector.hpp>
#include <GLM/gtx/vector_angle.hpp>

#include "Common.h"
#include "Shader.h"

struct AABB
{
	glm::vec3 min;
	glm::vec3 max;

	AABB() : min(0.0f), max(0.0f) {}
	AABB(glm::vec3 _min, glm::vec3 _max) : min(_min), max(_max) {}

	bool Intersects(const AABB& other) const
	{
		return (min.x <= other.max.x && max.x >= other.min.x) &&
		       (min.y <= other.max.y && max.y >= other.min.y) &&
		       (min.z <= other.max.z && max.z >= other.min.z);
	}
};

class Player
{
public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 velocity = glm::vec3(0.0f);

	// Camera angles (in degrees)
	float yaw = -90.0f;    // Horizontal rotation (look left/right)
	float pitch = 0.0f;    // Vertical rotation (look up/down)

	float width = 0.4f;   // Small enough to easily fit through 1-block holes
	float height = 1.8f;  // Standard player height

	float speed = 100.0f;
	float maxSpeed = 8.0f;
	float jumpForce = 8.0f;
	float gravity = 25.0f;
	float friction = 10.0f;
	float sensitivity = 0.1f;  // Mouse sensitivity (degrees per pixel)

	bool isGrounded = false;
	bool cursorLocked = true;
	bool leftMousePressed = false;
	bool isFlying = false;
	bool flyKeyPressed = false;
	bool firstMouse = true;
	double lastMouseX = 0.0;
	double lastMouseY = 0.0;

	Player() = default;
	Player(glm::vec3 _pos);

	AABB GetAABB() const;
	void Update(GLFWwindow* window, float deltaTime);
	void ApplyGravity(float deltaTime);
	void CheckInputs(GLFWwindow* window, float deltaTime);
	void CheckMouseInput(GLFWwindow* window);
	void CheckBlockInteraction(GLFWwindow* window);

	glm::vec3 GetEyePosition() const;
	glm::vec3 GetLookDirection() const;
	glm::vec3 GetForward() const;  // Horizontal forward (for movement)
	glm::vec3 GetRight() const;

	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
};