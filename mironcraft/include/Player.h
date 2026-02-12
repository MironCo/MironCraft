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
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);

	float width = 0.6f;
	float height = 1.8f;

	float speed = 100.0f;
	float maxSpeed = 8.0f;
	float jumpForce = 8.0f;
	float gravity = 25.0f;
	float friction = 10.0f;
	float sensitivity = 50.0f;

	bool isGrounded = false;
	bool cursorLocked = true;
	bool leftMousePressed = false;

	Player() = default;
	Player(glm::vec3 _pos);

	AABB GetAABB() const;
	void Update(GLFWwindow* window, float deltaTime);
	void ApplyGravity(float deltaTime);
	void CheckInputs(GLFWwindow* window, float deltaTime);
	void CheckMouseInput(GLFWwindow* window);
	void CheckBlockInteraction(GLFWwindow* window);

	glm::vec3 GetEyePosition() const;

	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
};