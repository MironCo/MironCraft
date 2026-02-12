#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/rotate_vector.hpp>
#include <GLM/gtx/vector_angle.hpp>

#include "Common.h"
#include "Shader.h"

class Camera
{
public:
	const float GRAVITY = 0.1f;

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);

	float speed = 0.095f;
	float sensitivity = 50.0f;
	bool cursorLocked = false;

	Camera() = default;
	Camera(glm::vec3 _pos);

	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
	void CheckCollisions();
	void CheckInputs(GLFWwindow* window);
};
