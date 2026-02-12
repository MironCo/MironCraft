#pragma once
#include "Common.h"
#include <glad/glad.h>
#include "Window.h"

class Application
{
public:
	static inline void Begin()
	{
		Window::Create();
		Application::End();
	}

	static inline void End()
	{
		Window::Close();
	}
};