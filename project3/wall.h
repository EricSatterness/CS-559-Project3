#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Box2D/Box2D.h>
#include "Globals.h"
#include "Cube.h"

class Wall
{
public:
	b2Body* body;

	Wall();
	bool Initialize(glm::vec3 center, float width, float height, float depth);
	void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size);
	void TakeDown();

private:
	float width, height, depth;
	Cube* cube;
};