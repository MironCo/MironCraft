#include "Player.h"
#include "Raycast.h"
#include "Collision.h"
#include "Renderer.h"
#include <cmath>

Player::Player(glm::vec3 _pos)
{
	position = _pos;
	cursorLocked = true;
	firstMouse = true;
}

AABB Player::GetAABB() const
{
	float halfWidth = width / 2.0f;
	glm::vec3 min(position.x - halfWidth, position.y - height, position.z - halfWidth);
	glm::vec3 max(position.x + halfWidth, position.y, position.z + halfWidth);
	return AABB(min, max);
}

void Player::Update(GLFWwindow* window, float deltaTime)
{
	CheckMouseInput(window);
	CheckInputs(window, deltaTime);
	CheckBlockInteraction(window);
	ApplyGravity(deltaTime);
}

glm::vec3 Player::GetEyePosition() const
{
	// Eye is at top of player minus a small offset for head clearance
	return position + glm::vec3(0.0f, -0.1f, 0.0f);
}

// Calculate look direction from pitch and yaw
glm::vec3 Player::GetLookDirection() const
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	return glm::normalize(direction);
}

// Horizontal forward direction (for movement, ignores pitch)
glm::vec3 Player::GetForward() const
{
	return glm::normalize(glm::vec3(cos(glm::radians(yaw)), 0.0f, sin(glm::radians(yaw))));
}

// Right direction (perpendicular to forward)
glm::vec3 Player::GetRight() const
{
	return glm::normalize(glm::cross(GetForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Player::CheckBlockInteraction(GLFWwindow* window)
{
	if (!cursorLocked) return;

	bool leftDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	bool rightDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	// Break block on left click
	if (leftDown && !leftMousePressed)
	{
		leftMousePressed = true;

		// Cast ray from eye position in look direction
		auto hit = Raycast::Cast(GetEyePosition(), GetLookDirection(), 5.0f);
		if (hit)
		{
			// Remove the block from world (collision + visual)
			Renderer::RemoveBlock(hit->blockPos.x, hit->blockPos.y, hit->blockPos.z);
		}
	}
	else if (!leftDown)
	{
		leftMousePressed = false;
	}

	// Place block on right click
	if (rightDown && !rightMousePressed)
	{
		rightMousePressed = true;

		// Cast ray from eye position in look direction
		auto hit = Raycast::Cast(GetEyePosition(), GetLookDirection(), 5.0f);
		if (hit)
		{
			// Place block adjacent to the hit face (use the normal)
			int placeX = hit->blockPos.x + hit->normal.x;
			int placeY = hit->blockPos.y + hit->normal.y;
			int placeZ = hit->blockPos.z + hit->normal.z;

			// Don't place if it would intersect with player
			AABB blockAABB(
				glm::vec3(placeX - 0.5f, placeY - 0.5f, placeZ - 0.5f),
				glm::vec3(placeX + 0.5f, placeY + 0.5f, placeZ + 0.5f)
			);
			if (!GetAABB().Intersects(blockAABB))
			{
				Renderer::AddBlock(placeX, placeY, placeZ, BlockType::LOG);
			}
		}
	}
	else if (!rightDown)
	{
		rightMousePressed = false;
	}
}

void Player::ApplyGravity(float deltaTime)
{
	if (isFlying)
	{
		// In fly mode, apply friction to all axes including Y
		float frictionFactor = std::exp(-friction * deltaTime);
		velocity *= frictionFactor;
		return;
	}

	if (!isGrounded)
	{
		velocity.y -= gravity * deltaTime;
	}
	else
	{
		if (velocity.y < 0)
			velocity.y = 0;
	}

	// Clamp horizontal velocity
	float horizontalSpeed = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	if (horizontalSpeed > maxSpeed)
	{
		float scale = maxSpeed / horizontalSpeed;
		velocity.x *= scale;
		velocity.z *= scale;
	}

	// Apply friction
	float frictionFactor = std::exp(-friction * deltaTime);
	velocity.x *= frictionFactor;
	velocity.z *= frictionFactor;
}

void Player::CheckInputs(GLFWwindow* window, float deltaTime)
{
	// Toggle fly mode with F
	bool fKeyDown = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
	if (fKeyDown && !flyKeyPressed)
	{
		isFlying = !isFlying;
		flyKeyPressed = true;
	}
	else if (!fKeyDown)
	{
		flyKeyPressed = false;
	}

	glm::vec3 forward = GetForward();
	glm::vec3 right = GetRight();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		velocity += speed * deltaTime * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		velocity -= speed * deltaTime * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		velocity -= speed * deltaTime * right;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		velocity += speed * deltaTime * right;
	}

	if (isFlying)
	{
		// Fly up/down with space/shift
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			velocity.y += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			velocity.y -= speed * deltaTime;
		}
	}
	else
	{
		// Normal jump
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded)
		{
			velocity.y = jumpForce;
			isGrounded = false;
		}
	}
}

void Player::CheckMouseInput(GLFWwindow* window)
{
	if (cursorLocked)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			cursorLocked = false;
			firstMouse = true;
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		if (firstMouse)
		{
			lastMouseX = mouseX;
			lastMouseY = mouseY;
			firstMouse = false;
		}

		// Calculate mouse delta
		double deltaX = mouseX - lastMouseX;
		double deltaY = lastMouseY - mouseY;  // Reversed: y goes down in screen coords
		lastMouseX = mouseX;
		lastMouseY = mouseY;

		// Apply sensitivity
		yaw += static_cast<float>(deltaX) * sensitivity;
		pitch += static_cast<float>(deltaY) * sensitivity;

		// Clamp pitch to prevent flipping
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}
	else
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			cursorLocked = true;
			firstMouse = true;
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Player::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	glm::vec3 eyePosition = GetEyePosition();
	glm::vec3 lookDir = GetLookDirection();

	glm::mat4 view = glm::lookAt(eyePosition, eyePosition + lookDir, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(FOVdeg), WindowData::GetAspectRatio(), nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
}
