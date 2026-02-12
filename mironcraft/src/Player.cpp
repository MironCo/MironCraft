#include "Player.h"
#include <cmath>

Player::Player(glm::vec3 _pos)
{
	position = _pos;
	cursorLocked = true;
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
	CheckInputs(window, deltaTime);
	CheckMouseInput(window);
	ApplyGravity(deltaTime);
}

void Player::ApplyGravity(float deltaTime)
{
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
	glm::vec3 forward = glm::normalize(glm::vec3(rotation.x, 0, rotation.z));
	glm::vec3 right = glm::normalize(glm::cross(rotation, upDirection));

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

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded)
	{
		velocity.y = jumpForce;
		isGrounded = false;
	}
}

void Player::CheckMouseInput(GLFWwindow* window)
{
	if (cursorLocked)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			cursorLocked = false;
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (WindowData::height / 2)) / WindowData::height;
		float rotY = sensitivity * (float)(mouseX - (WindowData::width / 2)) / WindowData::width;

		glm::vec3 newRotation = glm::rotate(rotation, glm::radians(-rotX), glm::normalize(glm::cross(rotation, upDirection)));

		if (!(glm::angle(newRotation, upDirection) <= glm::radians(1.0f)) || !(glm::angle(newRotation, -upDirection) <= glm::radians(10.0f)))
		{
			rotation = newRotation;
		}

		rotation = glm::rotate(rotation, glm::radians(-rotY), upDirection);
		glfwSetCursorPos(window, (WindowData::width / 2), (WindowData::height / 2));
	}
	else
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			cursorLocked = true;
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Player::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	glm::vec3 eyePosition = position + glm::vec3(0.0f, -0.2f, 0.0f);

	glm::mat4 view = glm::lookAt(eyePosition, eyePosition + rotation, upDirection);
	glm::mat4 proj = glm::perspective(glm::radians(FOVdeg), WindowData::aspectRatio, nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
}
