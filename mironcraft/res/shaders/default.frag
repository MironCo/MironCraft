#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 textureCoord;

uniform sampler2D tex0;

void main()
{
	vec4 texel = texture(tex0, textureCoord);
	FragColor = texel * vec4(color, 1.0f);
}
