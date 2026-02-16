#include "Window.h"
#include "Common.h"
#include "Shader.h"
#include "Player.h"
#include "Game.h"
#include "Renderer.h"
#include "Collision.h"
#include "Crosshair.h"
#include "DebugCamera.h"

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

	// Debug mode setup
	bool debugMode = false;
	bool debugKeyPressed = false;
	DebugCamera debugCamera(player.position + glm::vec3(5.0f, 5.0f, 5.0f));
	DebugRenderer debugRenderer;

	Clear();
	glfwSwapBuffers(window);

	float lastTime = static_cast<float>(glfwGetTime());
	float deltaTime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		float currentTime = static_cast<float>(glfwGetTime());
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// Toggle debug mode with F3
		bool f3Down = glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS;
		if (f3Down && !debugKeyPressed)
		{
			debugMode = !debugMode;
			debugKeyPressed = true;
			if (debugMode)
			{
				// Position debug camera near player
				debugCamera.position = player.position + glm::vec3(5.0f, 3.0f, 5.0f);
				debugCamera.firstMouse = true;
			}
			else
			{
				player.firstMouse = true;
			}
		}
		else if (!f3Down)
		{
			debugKeyPressed = false;
		}

		Clear();
		shaderProgram.Activate();

		if (debugMode)
		{
			// Debug mode: update debug camera, but still simulate player
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			debugCamera.Update(window, deltaTime);
			debugCamera.Matrix(60.0f, 0.15f, 500.0f, shaderProgram, "cameraMatrix");

			// Still update player physics (but don't read input)
			player.ApplyGravity(deltaTime);
			g_CollisionWorld.ResolveCollision(player, deltaTime);
		}
		else
		{
			// Normal mode
			player.Update(window, deltaTime);
			g_CollisionWorld.ResolveCollision(player, deltaTime);
			player.Matrix(60.0f, 0.15f, 100.0f, shaderProgram, "cameraMatrix");
		}

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

		// Draw debug visualizations in debug mode
		if (debugMode)
		{
			glm::vec3 lookDir = debugCamera.GetLookDirection();
			glm::mat4 view = glm::lookAt(debugCamera.position, debugCamera.position + lookDir, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 proj = glm::perspective(glm::radians(60.0f), WindowData::GetAspectRatio(), 0.15f, 500.0f);
			glm::mat4 viewProj = proj * view;

			glDisable(GL_DEPTH_TEST);
			debugRenderer.DrawPlayerDebug(player, viewProj);
			debugRenderer.DrawNearbyBlockColliders(player, viewProj);
			glEnable(GL_DEPTH_TEST);
		}

		// Draw crosshair on top (only in normal mode)
		if (!debugMode)
		{
			crosshair.Draw();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Window::Close()
{
	Renderer::Shutdown();
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
