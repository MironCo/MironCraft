#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
private:
	static GLFWwindow* window;
public:
	static void Create();
	static void Close();
	static void Clear();
	static GLFWwindow* Get();
};
