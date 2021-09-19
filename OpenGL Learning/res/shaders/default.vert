#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aColor;
layout (location = 2) in vec2 aTextureOffset;
layout (location = 3) in vec2 aTexture;

out vec3 color;
out vec2 textureCoord;

uniform mat4 cameraMatrix;

void main()
{
	gl_Position = cameraMatrix * vec4(aPos, 1.0);
	color = vec3(aColor, aColor, aColor);

	textureCoord = (aTexture / 4) + (aTextureOffset);
}