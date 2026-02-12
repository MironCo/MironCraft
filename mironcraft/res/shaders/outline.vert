#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aColor;
layout (location = 2) in vec2 aOffset;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 cameraMatrix;

void main()
{
	// Push vertices outward slightly along their direction from center
	vec3 center = floor(aPos) + 0.5;
	vec3 dir = normalize(aPos - center);
	vec3 expandedPos = aPos + dir * 0.02; // Outline thickness

	gl_Position = cameraMatrix * vec4(expandedPos, 1.0);
}
