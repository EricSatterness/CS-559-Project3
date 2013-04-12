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

class RectangularPrism : public Object
{
public:
	RectangularPrism();
	bool Initialize(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 t4, glm::vec3 b1, glm::vec3 b2, glm::vec3 b3, glm::vec3 b4);
	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size);
	void TakeDown();

private:
	void Square(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 color);
};
