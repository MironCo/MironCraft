#pragma once
#include <string>
#include <vector>
#include <cstring>

#include "Common.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include <GLM/glm.hpp>

enum BlockType
{
	GRASS = 0, STONE = 1, DIRT = 2, SAND = 3, WATER = 4, LOG = 5, LEAVES = 6
};

class Block
{
private:
	Texture texture;
	glm::mat4 model = glm::mat4(0.0f);
	BlockType blockType = BlockType::GRASS;
	GLfloat cachedVertices[192];

public:
	bool nextState = 1;
	glm::vec2 textureOffset = glm::vec2(0.0f);

	GLuint blockIndices[36] =
	{
		// Front
		0, 1, 2,
		0, 2, 3,
		// Back
		4, 5, 6,
		4, 6, 7,
		// Left
		8, 9, 10,
		8, 10, 11,
		// Right
		12, 13, 14,
		14, 15, 13,
		// Top
		16, 17, 18,
		18, 19, 17,
		// Bottom
		20, 21, 22,
		22, 23, 21
	};

	glm::vec3 position = glm::vec3(0.0f);
	VertexArray VAO;

	Block(glm::vec3 _pos, BlockType type);
	BlockType GetBlockType();
	GLfloat* GetNormalBlockVertsWithOffset(glm::vec3 offset, glm::vec2 _textureOffset);
	GLfloat* GetWaterBlockVertsWithOffset(glm::vec3 offset, glm::vec2 _textureOffset);
	GLfloat* GetGrassBlockVertsWithOffset(glm::vec3 offset);
	GLfloat* GetLogBlockVertsWithOffset(glm::vec3 offset);
	GLfloat* GetBlockVertsWithOffset(glm::vec3 offset, glm::vec2 _textureOffset, BlockType _type);
};
