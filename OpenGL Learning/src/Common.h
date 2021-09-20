#pragma once
#include <iostream>
#include <cmath>
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
const float WindowData::aspectRatio = (float)(WindowData::width) / (float)(WindowData::height);
const char* WindowData::name = "MironCraft";

float sqr(float number)
{
	return number * number;
}