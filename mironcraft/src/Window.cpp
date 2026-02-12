#include "Window.h"
#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Game.h"
#include "Renderer.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

GLFWwindow* Window::window = nullptr;

void Window::Create()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(WindowData::width, WindowData::height, WindowData::name, nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();

	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glEnable(GL_DEPTH_TEST);

	Shader shaderProgram("res/shaders/default.vert", "res/shaders/default.frag");
	Camera camera(glm::vec3(Game::worldCenter, 12.0f, Game::worldCenter));

	Game::Start(shaderProgram);

	Clear();
	glfwSwapBuffers(window);

	while (!glfwWindowShouldClose(window))
	{
		Clear();
		shaderProgram.Activate();

		camera.CheckInputs(window);
		camera.Matrix(60.0f, 0.02f, 100.0f, shaderProgram, "cameraMatrix");
		camera.CheckCollisions();

		Renderer::Draw(shaderProgram, camera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Window::Close()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::Clear()
{
	glClearColor(0.69f, 0.9f, 0.95f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* Window::Get()
{
	return window;
}
