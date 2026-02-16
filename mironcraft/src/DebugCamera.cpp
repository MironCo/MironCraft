#include "DebugCamera.h"
#include "Common.h"
#include "Collision.h"
#include <cmath>
#include <vector>

// DebugCamera implementation

DebugCamera::DebugCamera(glm::vec3 pos)
{
	position = pos;
}

glm::vec3 DebugCamera::GetLookDirection() const
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	return glm::normalize(direction);
}

glm::vec3 DebugCamera::GetForward() const
{
	return glm::normalize(glm::vec3(cos(glm::radians(yaw)), 0.0f, sin(glm::radians(yaw))));
}

glm::vec3 DebugCamera::GetRight() const
{
	return glm::normalize(glm::cross(GetForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void DebugCamera::Update(GLFWwindow* window, float deltaTime)
{
	glm::vec3 forward = GetForward();
	glm::vec3 right = GetRight();

	float currentSpeed = speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		currentSpeed *= 3.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += currentSpeed * deltaTime * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position -= currentSpeed * deltaTime * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position -= currentSpeed * deltaTime * right;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += currentSpeed * deltaTime * right;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position.y += currentSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		position.y -= currentSpeed * deltaTime;
	}

	// Mouse look
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (firstMouse)
	{
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		firstMouse = false;
	}

	double deltaX = mouseX - lastMouseX;
	double deltaY = lastMouseY - mouseY;
	lastMouseX = mouseX;
	lastMouseY = mouseY;

	yaw += static_cast<float>(deltaX) * sensitivity;
	pitch += static_cast<float>(deltaY) * sensitivity;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
}

void DebugCamera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	glm::vec3 lookDir = GetLookDirection();
	glm::mat4 view = glm::lookAt(position, position + lookDir, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(FOVdeg), WindowData::GetAspectRatio(), nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
}

// DebugRenderer implementation

DebugRenderer::DebugRenderer()
{
	lineShader = new Shader("res/shaders/debug.vert", "res/shaders/debug.frag");

	glGenVertexArrays(1, &lineVAO);
	glGenBuffers(1, &lineVBO);

	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	// Allocate space for up to 24 vertices (12 lines for a box)
	glBufferData(GL_ARRAY_BUFFER, 24 * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

DebugRenderer::~DebugRenderer()
{
	delete lineShader;
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);
}

void DebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, const glm::mat4& viewProj)
{
	float vertices[] = {
		start.x, start.y, start.z,
		end.x, end.y, end.z
	};

	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	lineShader->Activate();
	glUniformMatrix4fv(glGetUniformLocation(lineShader->shaderProgramID, "viewProj"), 1, GL_FALSE, glm::value_ptr(viewProj));
	glUniform3fv(glGetUniformLocation(lineShader->shaderProgramID, "lineColor"), 1, glm::value_ptr(color));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void DebugRenderer::DrawAABB(const AABB& aabb, const glm::vec3& color, const glm::mat4& viewProj)
{
	glm::vec3 min = aabb.min;
	glm::vec3 max = aabb.max;

	// 8 corners of the box
	glm::vec3 corners[8] = {
		{min.x, min.y, min.z},
		{max.x, min.y, min.z},
		{max.x, min.y, max.z},
		{min.x, min.y, max.z},
		{min.x, max.y, min.z},
		{max.x, max.y, min.z},
		{max.x, max.y, max.z},
		{min.x, max.y, max.z}
	};

	// 12 edges
	int edges[12][2] = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},  // Bottom
		{4, 5}, {5, 6}, {6, 7}, {7, 4},  // Top
		{0, 4}, {1, 5}, {2, 6}, {3, 7}   // Verticals
	};

	// Build vertex buffer for all edges
	float vertices[24 * 3];
	for (int i = 0; i < 12; i++)
	{
		vertices[i * 6 + 0] = corners[edges[i][0]].x;
		vertices[i * 6 + 1] = corners[edges[i][0]].y;
		vertices[i * 6 + 2] = corners[edges[i][0]].z;
		vertices[i * 6 + 3] = corners[edges[i][1]].x;
		vertices[i * 6 + 4] = corners[edges[i][1]].y;
		vertices[i * 6 + 5] = corners[edges[i][1]].z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	lineShader->Activate();
	glUniformMatrix4fv(glGetUniformLocation(lineShader->shaderProgramID, "viewProj"), 1, GL_FALSE, glm::value_ptr(viewProj));
	glUniform3fv(glGetUniformLocation(lineShader->shaderProgramID, "lineColor"), 1, glm::value_ptr(color));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(0);
}

void DebugRenderer::DrawPlayerDebug(const Player& player, const glm::mat4& viewProj)
{
	// Draw player AABB in green
	DrawAABB(player.GetAABB(), glm::vec3(0.0f, 1.0f, 0.0f), viewProj);

	// Draw look direction as a line from eye position
	glm::vec3 eyePos = player.GetEyePosition();
	glm::vec3 lookEnd = eyePos + player.GetLookDirection() * 3.0f;
	DrawLine(eyePos, lookEnd, glm::vec3(1.0f, 1.0f, 0.0f), viewProj);

	// Draw forward direction (horizontal movement)
	glm::vec3 forwardEnd = eyePos + player.GetForward() * 2.0f;
	DrawLine(eyePos, forwardEnd, glm::vec3(0.0f, 0.0f, 1.0f), viewProj);

	// Draw right direction
	glm::vec3 rightEnd = eyePos + player.GetRight() * 1.5f;
	DrawLine(eyePos, rightEnd, glm::vec3(1.0f, 0.0f, 0.0f), viewProj);
}

void DebugRenderer::DrawNearbyBlockColliders(const Player& player, const glm::mat4& viewProj)
{
	// Draw collision AABBs for blocks near the player
	glm::vec3 pos = player.position;
	int radius = 3;

	int centerX = static_cast<int>(std::round(pos.x));
	int centerY = static_cast<int>(std::round(pos.y));
	int centerZ = static_cast<int>(std::round(pos.z));

	for (int x = centerX - radius; x <= centerX + radius; x++)
	{
		for (int y = centerY - radius - 2; y <= centerY + radius; y++)
		{
			for (int z = centerZ - radius; z <= centerZ + radius; z++)
			{
				if (g_CollisionWorld.HasBlock(x, y, z))
				{
					AABB blockAABB = g_CollisionWorld.GetBlockAABB(x, y, z);
					DrawAABB(blockAABB, glm::vec3(1.0f, 0.5f, 0.0f), viewProj);  // Orange for blocks
				}
			}
		}
	}
}
