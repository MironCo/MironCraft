#include "Crosshair.h"
#include "Common.h"

Crosshair::Crosshair() : shader("res/shaders/ui.vert", "res/shaders/ui.frag")
{
	// Crosshair size in NDC (normalized device coordinates)
	float size = 0.02f;
	float thickness = 0.003f;

	// Two lines: horizontal and vertical
	float vertices[] = {
		// Horizontal line
		-size, -thickness,
		 size, -thickness,
		 size,  thickness,
		-size, -thickness,
		 size,  thickness,
		-size,  thickness,

		// Vertical line
		-thickness, -size,
		 thickness, -size,
		 thickness,  size,
		-thickness, -size,
		 thickness,  size,
		-thickness,  size,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Crosshair::~Crosshair()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Crosshair::Draw()
{
	// Disable depth testing for UI
	glDisable(GL_DEPTH_TEST);

	shader.Activate();
	glUniform3f(glGetUniformLocation(shader.shaderProgramID, "uiColor"), 1.0f, 1.0f, 1.0f);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);

	// Re-enable depth testing
	glEnable(GL_DEPTH_TEST);
}
