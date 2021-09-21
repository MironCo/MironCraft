#pragma once
#include <string>
#include <vector>

#include "Common.h" 
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"

enum BlockType
{
	GRASS = 0, STONE = 1, DIRT = 2, SAND = 3, WATER = 4, LOG = 5, LEAVES = 6
};

class Block
{
private:
	//Block Vertices
	Texture texture;
	glm::mat4 model = glm::mat4(0.0f);
	
	glm::vec3 index;
	BlockType blockType;
	
public:
	bool nextState = 1;
	glm::vec2 textureOffset;

	//Block Indices for vertices
	GLuint blockIndices[36] =
	{
		//Front
		0, 1, 2,
		0, 2, 3,

		//Back
		4, 5, 6,
		4, 6, 7,

		//Left
		8, 9, 10,
		8, 10, 11,

		//Right
		12, 13, 14,
		14, 15, 13,

		//Top
		16, 17, 18,
		18, 19, 17,

		//Bottom
		20, 21, 22,
		22, 23, 21
	};

	glm::vec3 position;
	VertexArray VAO;

	Block(glm::vec3 _pos, BlockType type)
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
	void Delete()
	{
		delete(this);
	}
	BlockType GetBlockType()
	{
		return blockType;
	}
	GLfloat* GetNormalBlockVertsWithOffset(glm::vec3 offset, glm::vec2 _textureOffset)
	{
		GLfloat blockOffsetVertices[192] =
		{
			//COORDINATES													RGB     TEXTURE OFFSETS					 TEXTURE COORDINATES
			//Front 						
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f, //0
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0, 0.0f,  //1
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //2
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f, //3

			//Back							
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f, //4
			(offset.x) + 0.5f,	(offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0, 0.0f,  //5
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //6
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f, //7

			//Left							
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f,			//8
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 0.0f,//9
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //10
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f,//11

			//Right							
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 0.0f, //12
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f,//13
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //14
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f, //15

			//Top							
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 0.0f, //16
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f, //17
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //18
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f, //19

			//Bottom						
			(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 0.0f,			//20
			(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0, 0.0f, //21
			(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //22
			(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0, 1.0f //23
		};
		return blockOffsetVertices;
	}

	GLfloat* GetWaterBlockVertsWithOffset(glm::vec3 offset, glm::vec2 _textureOffset)
	{
		GLfloat blockOffsetVertices[192] =
		{
			//COORDINATES													RGB     TEXTURE OFFSETS					 TEXTURE COORDINATES
			//Front 						
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f, //0
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0, 0.0f,  //1
			(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //2
			(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + 0.5f,		0.85f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f, //3

			//Back							
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f, //4
			(offset.x) + 0.5f,	(offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0, 0.0f,  //5
			(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //6
			(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f, //7

			//Left							
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f,			//8
			(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + 0.5f,		0.65,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 0.0f,//9
			(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + -0.5f,		0.65,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //10
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f,//11

			//Right							
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 0.0f, //12
			(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + 0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f,//13
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //14
			(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + -0.5f,		0.65f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f, //15

			//Top							
			(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + 0.5f,		1.0f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 0.0f, //16
			(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + 0.5f,		1.0f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 0.0f, //17
			(offset.x) + 0.5f,  (offset.y) + 0.3f,  (offset.z) + -0.5f,		1.0f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //18
			(offset.x) + -0.5f, (offset.y) + 0.3f,  (offset.z) + -0.5f,		1.0f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0f, 1.0f, //19

			//Bottom						
			(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 0.0f,			//20
			(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0, 0.0f, //21
			(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 1.0f, 1.0f, //22
			(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	_textureOffset.x * 0.25, _textureOffset.y * 0.25, 0.0, 1.0f //23
		};
		return blockOffsetVertices;
	}

	GLfloat* GetGrassBlockVertsWithOffset(glm::vec3 offset)
	{
		GLfloat blockOffsetVertices[192] =
		{
			//COORDINATES													RGB     TEXTURE OFFSETS					 TEXTURE COORDINATES
			//Front 						
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	1 * 0.25, 1 * 0.25, 1.0f, 1.0f, //0
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	1 * 0.25, 1 * 0.25, 0.0, 1.0f,  //1
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	1 * 0.25, 1 * 0.25, 0.0f, 0.0f, //2
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	1 * 0.25, 1 * 0.25, 1.0f, 0.0f, //3

			//Back							
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	1 * 0.25, 1 * 0.25, 1.0f, 1.0f, //4
			(offset.x) + 0.5f,	(offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	1 * 0.25, 1 * 0.25, 0.0, 1.0f,  //5
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	1 * 0.25, 1 * 0.25, 0.0f, 0.0f, //6
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	1 * 0.25, 1 * 0.25, 1.0f, 0.0f, //7

			//Left							
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65,	1 * 0.25, 1 * 0.25, 1.0f, 1.0f,			//8
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65,	1 * 0.25, 1 * 0.25, 1.0f, 0.0f,//9
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65,	1 * 0.25, 1 * 0.25, 0.0f, 0.0f, //10
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65,	1 * 0.25, 1 * 0.25, 0.0f, 1.0f,//11

			//Right							
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	1 * 0.25, 1 * 0.25, 0.0f, 1.0f, //12
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65f,	1 * 0.25, 1 * 0.25, 0.0f, 0.0f,//13
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	1 * 0.25, 1 * 0.25, 1.0f, 1.0f, //14
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	1 * 0.25, 1 * 0.25, 1.0f, 0.0f, //15

			//Top							
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	0 * 0.25, 0 * 0.25, 1.0f, 0.0f, //16
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	0 * 0.25, 0 * 0.25, 0.0f, 0.0f, //17
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	0 * 0.25, 0 * 0.25, 1.0f, 1.0f, //18
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	0 * 0.25, 0 * 0.25, 0.0f, 1.0f, //19

			//Bottom						
			(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	3 * 0.25, 0 * 0.25, 1.0f, 0.0f,			//20
			(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	3 * 0.25, 0 * 0.25, 0.0, 0.0f, //21
			(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	3 * 0.25, 0 * 0.25, 1.0f, 1.0f, //22
			(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	3 * 0.25, 0 * 0.25, 0.0, 1.0f //23
		};
		return blockOffsetVertices;
	}

	GLfloat* GetLogBlockVertsWithOffset(glm::vec3 offset)
	{
		GLfloat blockOffsetVertices[192] =
		{
			//COORDINATES													RGB     TEXTURE OFFSETS					 TEXTURE COORDINATES
			//Front 						
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	2 * 0.25, 1 * 0.25, 1.0f, 1.0f, //0
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.85f,	2 * 0.25, 1 * 0.25, 0.0, 1.0f,  //1
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	2 * 0.25, 1 * 0.25, 0.0f, 0.0f, //2
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.85f,	2 * 0.25, 1 * 0.25, 1.0f, 0.0f, //3

			//Back							
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	2 * 0.25, 1 * 0.25, 1.0f, 1.0f, //4
			(offset.x) + 0.5f,	(offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	2 * 0.25, 1 * 0.25, 0.0, 1.0f,  //5
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	2 * 0.25, 1 * 0.25, 0.0f, 0.0f, //6
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	2 * 0.25, 1 * 0.25, 1.0f, 0.0f, //7

			//Left							
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65,	2 * 0.25, 1 * 0.25, 1.0f, 1.0f,			//8
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65,	2 * 0.25, 1 * 0.25, 1.0f, 0.0f,//9
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65,	2 * 0.25, 1 * 0.25, 0.0f, 0.0f, //10
			(offset.x) + -0.5f, (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65,	2 * 0.25, 1 * 0.25, 0.0f, 1.0f,//11

			//Right							
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + 0.5f,		0.65f,	2 * 0.25, 1 * 0.25, 0.0f, 1.0f, //12
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		0.65f,	2 * 0.25, 1 * 0.25, 0.0f, 0.0f,//13
			(offset.x) + 0.5f,  (offset.y) + -0.5f, (offset.z) + -0.5f,		0.65f,	2 * 0.25, 1 * 0.25, 1.0f, 1.0f, //14
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		0.65f,	2 * 0.25, 1 * 0.25, 1.0f, 0.0f, //15

			//Top							
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	3 * 0.25, 1 * 0.25, 1.0f, 0.0f, //16
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + 0.5f,		1.0f,	3 * 0.25, 1 * 0.25, 0.0f, 0.0f, //17
			(offset.x) + 0.5f,  (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	3 * 0.25, 1 * 0.25, 1.0f, 1.0f, //18
			(offset.x) + -0.5f, (offset.y) + 0.5f,  (offset.z) + -0.5f,		1.0f,	3 * 0.25, 1 * 0.25, 0.0f, 1.0f, //19

			//Bottom						
			(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	3 * 0.25, 1 * 0.25, 1.0f, 0.0f,			//20
			(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + 0.5f,		0.4f,	3 * 0.25, 1 * 0.25, 0.0, 0.0f, //21
			(offset.x) + -0.5f, (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	3 * 0.25, 1 * 0.25, 1.0f, 1.0f, //22
			(offset.x) + 0.5f,  (offset.y) + -0.5f,  (offset.z) + -0.5f,	0.4f,	3 * 0.25, 1 * 0.25, 0.0, 1.0f //23
		};
		return blockOffsetVertices;
	}

	GLfloat* GetBlockVertsWithOffset(glm::vec3 offset, glm::vec2 _textureOffset, BlockType _type)
	{
		if (_type == BlockType::WATER)
		{
			return GetWaterBlockVertsWithOffset(offset, _textureOffset);
		} else if (_type == BlockType::GRASS)
		{ 
			return GetGrassBlockVertsWithOffset(offset);	
		}
		else if (_type == BlockType::LOG)
		{
			return GetLogBlockVertsWithOffset(offset);
		}
		else {
			return GetNormalBlockVertsWithOffset(offset, _textureOffset);
		}
	}
};