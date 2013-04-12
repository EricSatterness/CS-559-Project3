/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code it directly copied
*/

#include <iostream>
#include "RectangularPrism.h"

using namespace std;
using namespace glm;

RectangularPrism::RectangularPrism() : Object()
{
	color = vec3(MakeColor(255, 69, 0, 1.0f));
}

void RectangularPrism::Square(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, vec3 color)
{
	VertexAttributesPCN cur_vertex_top, cur_vertex_bottom , nxt_vertex_top,nxt_vertex_bottom;

	cur_vertex_top.position = p1;
	cur_vertex_bottom.position = p2;
	nxt_vertex_top.position = p4;
	nxt_vertex_bottom.position = p3;

	vec3 n = normalize(cross(p1-p2, p1-p3));
	cur_vertex_bottom.normal = n;
	cur_vertex_top.normal = n;
	nxt_vertex_bottom.normal = n;
	nxt_vertex_top.normal = n;

	cur_vertex_bottom.color = color;
	cur_vertex_top.color = color;
	nxt_vertex_bottom.color = color;
	nxt_vertex_top.color = color;

	this->vertices.push_back(cur_vertex_top);
	this->vertices.push_back(nxt_vertex_bottom);
	this->vertices.push_back(cur_vertex_bottom);
	
	this->vertex_indices.push_back(this->vertices.size() - 3);
	this->vertex_indices.push_back(this->vertices.size() - 1);
	this->vertex_indices.push_back(this->vertices.size() - 2);

	this->BuildNormalVisualizationGeometry();

	this->vertices.push_back(cur_vertex_top);
	this->vertices.push_back(nxt_vertex_top);
	this->vertices.push_back(nxt_vertex_bottom);
	
	this->vertex_indices.push_back(this->vertices.size() - 3);
	this->vertex_indices.push_back(this->vertices.size() - 1);
	this->vertex_indices.push_back(this->vertices.size() - 2);

	this->BuildNormalVisualizationGeometry();
}
bool RectangularPrism::Initialize(vec3 t1, vec3 t2, vec3 t3, vec3 t4, vec3 b1, vec3 b2, vec3 b3, vec3 b4)
{
	if (this->GLReturnedError("RectangularPrism::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;
	

	Square(t1, b1, b2, t2, color);
	Square(b4, t4, t3, b3, color);
	
	Square(t4, t1, t2, t3, color);
	Square(b3, b2, b1, b4, color);

	Square(t1, t4, b4, b1, color);
	Square(b2, b3, t3, t2, color);



	if (!this->PostGLInitialize(&this->vertex_array_handle, &this->vertex_coordinate_handle, this->vertices.size() * sizeof(VertexAttributesPCN), &this->vertices[0]))
		return false;

	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCN), (GLvoid *) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCN), (GLvoid *) (sizeof(vec3) * 2));	// Note offset - legacy of older code
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCN), (GLvoid *) (sizeof(vec3) * 1));	// Same
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (this->normal_vertices.size() > 0)
	{
		if (!this->PostGLInitialize(&this->normal_array_handle, &this->normal_coordinate_handle, this->normal_vertices.size() * sizeof(VertexAttributesP), &this->normal_vertices[0]))
			return false;

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesP), (GLvoid *) 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	if (this->GLReturnedError("Rectangular Prism ::Initialize - on exit"))
		return false;

	return true;
}

void RectangularPrism::TakeDown()
{
	this->vertices.clear();
	super::TakeDown();
}

void RectangularPrism::Draw(const mat4 & projection, mat4 modelview, const ivec2 &size)
{
	super::Draw(projection, modelview, size);
}