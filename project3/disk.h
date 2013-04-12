/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code and is directly copied
*/

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "object.h"
#include "shader.h"

class Disk : public Object
{
public:
	Disk();
	bool Initialize(float innerRadius, float outerRadius, int slices, int loops);
	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size);
	void TakeDown();
};
