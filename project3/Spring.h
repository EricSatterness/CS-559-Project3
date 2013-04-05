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

class Spring : public Object
{
public:
	Spring();
	bool Initialize(float innerRadius, float outerRadius, int nsides, int rings, int loops, float heightPerLoop);
	virtual void Draw(const glm::ivec2 & size);
	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size);
	void TakeDown();
	Shader shader;
	Shader solid_color;
	glm::vec3 color;
	bool solidColor;
private:
	glm::vec3 normalCalc(int i, float delta, glm::mat4 m);
	void BuildNormalVisualizationGeometry();
	glm::vec4 colors[2];
	std::vector<VertexAttributesPCN> vertices;
	typedef Object super;
};
