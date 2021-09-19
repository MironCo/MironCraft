#pragma once

#include <stb/stb_image.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Common.h"
#include "Shader.h"

#include "Game.h"
#include "Renderer.h"

class Window
{
private:
	static GLFWwindow* window;
public:
	static inline void Create()
	{
		//Start GLFW
		glfwInit();

		//Setup the window hints
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		//Create Window
		window = glfwCreateWindow(WindowData::width, WindowData::height, WindowData::name, NULL, NULL);

		//Check for window Error
		if (window == NULL)
		{
			glfwTerminate();
		}
		//Make the window part of the current opengl context
		glfwMakeContextCurrent(window);

		//Start The OpenGl
		gladLoadGL();

		//Set the viewport
		glViewport(0, 0, WindowData::width, WindowData::height);
		glMatrixMode(GL_PROJECTION);
		glOrtho(-WindowData::aspectRatio, WindowData::aspectRatio, -1, 1, -1, 1);

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glFrontFace(GL_CCW);

		//glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		Shader shaderProgram = Shader("res/shaders/default.vert", "res/shaders/default.frag");
		Camera camera = Camera(glm::vec3(Game::worldCenter, 12.0f, Game::worldCenter));

		Game::Start(shaderProgram);
		
		//Clear Window for the first time
		Window::Clear();
		glfwSwapBuffers(window);

		//Main While Loop
		while (!glfwWindowShouldClose(window))
		{
			// Specify the color of the background
			Window::Clear();
			// Tell OpenGL which Shader Program we want to use
			shaderProgram.Activate();

			//block->Draw(shaderProgram, camera);
			camera.CheckInputs(window);
			camera.Matrix(60.0f, 0.02f, 100.0f, shaderProgram, "cameraMatrix");

			Renderer::Draw(shaderProgram, camera);

			// Swap the back buffer with the front buffer
			glfwSwapBuffers(window);
			// Take care of all GLFW events
			glfwPollEvents();
		}

		Window::Close();
	}

	static inline void Close()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	static inline void Clear()
	{
		glClearColor(0.69f, 0.9f, 0.95f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
};

GLFWwindow* Window::window;