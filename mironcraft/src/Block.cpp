#include "Block.h"

Block::Block(glm::vec3 _pos, BlockType type)
{
	position = _pos;
	blockType = type;

	if (type == BlockType::GRASS)
	{
		textureOffset.x = 0;
		textureOffset.y = 0;
	}
	else if (type == BlockType::STONE)
	{
		textureOffset.x = 1;
		textureOffset.y = 0;
	}
	else if (type == BlockType::DIRT)
	{
		textureOffset.x = 3;
		textureOffset.y = 0;
	}
	else if (type == BlockType::SAND)
	{
		textureOffset.x = 2;
		textureOffset.y = 0;
	}
	else if (type == BlockType::WATER)
	{
		textureOffset.x = 0;
		textureOffset.y = 1;
	}
	else if (type == BlockType::LOG)
	{
		textureOffset.x = 2;
		textureOffset.y = 1;
	}
	else if (type == BlockType::LEAVES)
	{
		textureOffset.x = 0;
		textureOffset.y = 2;
	}
}

BlockType Block::GetBlockType()
{
	return blockType;
}

GLfloat* Block::GetNormalBlockVertsWithOffset(glm::vec3 offset, glm::vec2 _textureOffset)
{
	GLfloat blockOffsetVertices[192] =
	{
		// Front
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Back
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,	(offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Left
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Right
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Top
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Bottom
		(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f
	};
	memcpy(cachedVertices, blockOffsetVertices, sizeof(blockOffsetVertices));
	return cachedVertices;
}

GLfloat* Block::GetWaterBlockVertsWithOffset(glm::vec3 offset, glm::vec2 _textureOffset)
{
	GLfloat blockOffsetVertices[192] =
	{
		// Front (water is shorter - 0.3 instead of 0.5 on top)
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Back
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,	(offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Left
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Right
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Top
		(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + 0.5f,		1.0f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + 0.5f,		1.0f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + -0.5f,		1.0f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + -0.5f,		1.0f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f,

		// Bottom
		(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	_textureOffset.x * 0.25f, _textureOffset.y * 0.25f, 0.0f, 1.0f
	};
	memcpy(cachedVertices, blockOffsetVertices, sizeof(blockOffsetVertices));
	return cachedVertices;
}

GLfloat* Block::GetGrassBlockVertsWithOffset(glm::vec3 offset)
{
	GLfloat blockOffsetVertices[192] =
	{
		// Front (side texture at 1,1 in atlas)
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	1 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	1 * 0.25f, 1 * 0.25f, 0.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	1 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	1 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,

		// Back
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,	(offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 0.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,

		// Left
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 0.0f, 1.0f,

		// Right
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 0.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	1 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,

		// Top (grass top texture at 0,0 in atlas)
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	0 * 0.25f, 0 * 0.25f, 1.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	0 * 0.25f, 0 * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	0 * 0.25f, 0 * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	0 * 0.25f, 0 * 0.25f, 0.0f, 1.0f,

		// Bottom (dirt texture at 3,0 in atlas)
		(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	3 * 0.25f, 0 * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	3 * 0.25f, 0 * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	3 * 0.25f, 0 * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	3 * 0.25f, 0 * 0.25f, 0.0f, 1.0f
	};
	memcpy(cachedVertices, blockOffsetVertices, sizeof(blockOffsetVertices));
	return cachedVertices;
}

GLfloat* Block::GetLogBlockVertsWithOffset(glm::vec3 offset)
{
	GLfloat blockOffsetVertices[192] =
	{
		// Front (log side texture at 2,1 in atlas)
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	2 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	2 * 0.25f, 1 * 0.25f, 0.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	2 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	2 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,

		// Back
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,	(offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 0.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,

		// Left
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 0.0f, 1.0f,

		// Right
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 0.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	2 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,

		// Top (log top texture at 3,1 in atlas)
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	3 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	3 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	3 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	3 * 0.25f, 1 * 0.25f, 0.0f, 1.0f,

		// Bottom
		(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	3 * 0.25f, 1 * 0.25f, 1.0f, 0.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	3 * 0.25f, 1 * 0.25f, 0.0f, 0.0f,
		(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	3 * 0.25f, 1 * 0.25f, 1.0f, 1.0f,
		(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	3 * 0.25f, 1 * 0.25f, 0.0f, 1.0f
	};
	memcpy(cachedVertices, blockOffsetVertices, sizeof(blockOffsetVertices));
	return cachedVertices;
}

GLfloat* Block::GetBlockVertsWithOffset(glm::vec3 offset, glm::vec2 _textureOffset, BlockType _type)
{
	if (_type == BlockType::WATER)
	{
		return GetWaterBlockVertsWithOffset(offset, _textureOffset);
	}
	else if (_type == BlockType::GRASS)
	{
		return GetGrassBlockVertsWithOffset(offset);
	}
	else if (_type == BlockType::LOG)
	{
		return GetLogBlockVertsWithOffset(offset);
	}
	else
	{
		return GetNormalBlockVertsWithOffset(offset, _textureOffset);
	}
}
