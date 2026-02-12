#include "Common.h"

const float WindowData::aspectRatio = (float)(WindowData::width) / (float)(WindowData::height);
const char* WindowData::name = "MironCraft";
int WindowData::framebufferWidth = 1600;
int WindowData::framebufferHeight = 900;

float sqr(float number)
{
	return number * number;
}
