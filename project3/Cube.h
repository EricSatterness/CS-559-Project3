/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code it directly copied
*/

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "object.h"
#include "shader.h"

class Cube : public Object
{
public:
	Cube();
	bool Initialize(float size);
	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size);
	void TakeDown();

private:
	void Square(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 color);
};
