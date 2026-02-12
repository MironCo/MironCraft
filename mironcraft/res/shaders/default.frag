#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 color;
in vec2 textureCoord;
in vec4 fragPosLightSpace;
in float faceBrightness;

uniform sampler2D tex0;
uniform sampler2D shadowMap;
uniform vec3 lightDir;
uniform vec3 viewPos;

// Derive normal from face brightness (each face has unique brightness)
vec3 GetNormalFromBrightness(float brightness)
{
	// Top: 1.0, Front: 0.85, Back: 0.65, Sides: 0.65, Bottom: 0.4
	if (brightness > 0.95) return vec3(0.0, 1.0, 0.0);        // Top
	if (brightness > 0.8) return vec3(0.0, 0.0, 1.0);         // Front (+Z)
	if (brightness > 0.6) return vec3(0.0, 0.0, -1.0);        // Back/Sides (-Z, approx)
	if (brightness > 0.5) return vec3(1.0, 0.0, 0.0);         // Sides
	return vec3(0.0, -1.0, 0.0);                               // Bottom
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
		return 0.0;

	float currentDepth = projCoords.z;
	float bias = 0.015;

	// PCF for softer shadows
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}

void main()
{
	vec4 texel = texture(tex0, textureCoord);

	// Get normal from face brightness
	vec3 normal = GetNormalFromBrightness(faceBrightness);

	// Light direction (pointing towards light)
	vec3 lightDirection = normalize(-lightDir);

	// Ambient - base lighting so nothing is pure black
	float ambient = 0.75;

	// Diffuse - faces lit based on angle to sun
	float diff = max(dot(normal, lightDirection), 0.0);
	float diffuse = diff * 0.4;

	// Specular (Blinn-Phong) - shiny highlights
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDirection + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	float specular = spec * 0.2;

	// Shadow
	float shadow = ShadowCalculation(fragPosLightSpace);
	float shadowFactor = 1.0 - shadow;

	// Final lighting: ambient always, diffuse+specular only when not in shadow
	float lighting = ambient + (diffuse + specular) * shadowFactor;

	// Apply lighting to texture (ignore the baked face brightness in 'color')
	FragColor = texel * vec4(vec3(lighting), 1.0);
}
