#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Player.h"

class DebugCamera
{
public:
	glm::vec3 position = glm::vec3(0.0f);
	float yaw = -90.0f;
	float pitch = 0.0f;
	float speed = 20.0f;
	float sensitivity = 0.1f;

	bool firstMouse = true;
	double lastMouseX = 0.0;
	double lastMouseY = 0.0;

	DebugCamera() = default;
	DebugCamera(glm::vec3 pos);

	void Update(GLFWwindow* window, float deltaTime);
	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);

	glm::vec3 GetLookDirection() const;
	glm::vec3 GetForward() const;
	glm::vec3 GetRight() const;
};

class DebugRenderer
{
private:
	GLuint lineVAO = 0;
	GLuint lineVBO = 0;
	Shader* lineShader = nullptr;

public:
	DebugRenderer();
	~DebugRenderer();

	void DrawAABB(const AABB& aabb, const glm::vec3& color, const glm::mat4& viewProj);
	void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, const glm::mat4& viewProj);
	void DrawPlayerDebug(const Player& player, const glm::mat4& viewProj);
	void DrawNearbyBlockColliders(const Player& player, const glm::mat4& viewProj);
};
