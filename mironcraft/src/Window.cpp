#include "Window.h"
#include "Common.h"
#include "Shader.h"
#include "Player.h"
#include "Game.h"
#include "Renderer.h"
#include "Collision.h"
#include "Crosshair.h"

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

	glfwGetFramebufferSize(window, &WindowData::framebufferWidth, &WindowData::framebufferHeight);
	glViewport(0, 0, WindowData::framebufferWidth, WindowData::framebufferHeight);

	glEnable(GL_DEPTH_TEST);

	Shader shaderProgram("res/shaders/default.vert", "res/shaders/default.frag");
	Player player(glm::vec3(Game::worldCenter, 50.0f, Game::worldCenter));

	Renderer::Init();
	Game::Start(shaderProgram);

	Crosshair crosshair;

	Clear();
	glfwSwapBuffers(window);

	float lastTime = static_cast<float>(glfwGetTime());
	float deltaTime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		float currentTime = static_cast<float>(glfwGetTime());
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		Clear();
		shaderProgram.Activate();

		player.Update(window, deltaTime);
		g_CollisionWorld.ResolveCollision(player, deltaTime);
		player.Matrix(60.0f, 0.02f, 100.0f, shaderProgram, "cameraMatrix");

		// Q/E to rotate light
		float lightRotSpeed = 45.0f; // degrees per second
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			Renderer::RotateLight(-lightRotSpeed * deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			Renderer::RotateLight(lightRotSpeed * deltaTime);
		}

		Renderer::Draw(shaderProgram, player);

		// Draw crosshair on top
		crosshair.Draw();

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
