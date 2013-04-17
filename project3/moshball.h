#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Box2D/Box2D.h>
#include "Globals.h"
#include "Sphere.h"

class Moshball
{
public:
	b2Body* body;

	Moshball();
	bool Initialize(glm::vec3 center, float radius, int slices, int stacks);
	void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size);
	void startContact();
	void TakeDown();

private:
	Sphere* sphere;
};