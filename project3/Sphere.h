#pragma once
/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code and is directly copied
*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "object.h"
#include "shader.h"

class Sphere : public Object
{
public:
	Sphere();
	Sphere(glm::vec3 color);
	bool Initialize(float radius, int slices, int stacks);
	void ChangeColor(glm::vec3 color);
	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size);
	void TakeDown();
};
