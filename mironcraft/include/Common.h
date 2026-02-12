#pragma once
#include <iostream>
#include <cmath>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct WindowData
{
	static const int width = 1600;
	static const int height = 900;
	static const char* name;
	static const int fpsCap = 60;
	static const float aspectRatio;
};

float sqr(float number);
