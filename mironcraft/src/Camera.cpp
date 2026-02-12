#include "Camera.h"

Camera::Camera(glm::vec3 _pos)
{
	position = _pos;
	cursorLocked = true;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	view = glm::lookAt(position, position + rotation, upDirection);
	proj = glm::perspective(glm::radians(FOVdeg), WindowData::aspectRatio, nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
}

void Camera::CheckCollisions()
{
	// Collision checking placeholder
}

void Camera::CheckInputs(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += speed * glm::vec3(rotation.x, 0, rotation.z);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position += speed * -glm::normalize(glm::cross(rotation, upDirection));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position += speed * -glm::vec3(rotation.x, 0, rotation.z);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += speed * glm::normalize(glm::cross(rotation, upDirection));
	}

	// Up and Down
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += speed * upDirection;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		position += speed * -upDirection;
	}

	if (cursorLocked == true)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			cursorLocked = false;
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		double mouseX;
		double mouseY;

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
