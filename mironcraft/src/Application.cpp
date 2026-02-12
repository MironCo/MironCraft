#include "Application.h"
#include "Window.h"

namespace Application
{
	void Run()
	{
		Window::Create();
		Window::Close();
	}
}
