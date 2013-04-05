/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code it directly copied
*/

#include <iostream>
#include "Square.h"

using namespace std;
using namespace glm;

Square::Square() : Object()
{
	vec4 lighter_color(MakeColor(255, 69, 0, 1.0f));
	vec4 darker_color = vec4(vec3(lighter_color) * 2.0f / 3.0f, 1.0f);
	this->colors[0] = darker_color;
	this->colors[1] = lighter_color;
	color = vec3(colors[0]);
	solidColor = true;
}

inline int ColorIndex(int i, int slices)
{
	return (i / (slices / 4)) % 2;
}

inline int PreviousSlice(int i, int slices)
{
	return (i == 0) ? slices - 1 : i - 1;
}

void Square::BuildNormalVisualizationGeometry()
{
	const float normal_scalar = 0.125f;
	for (int j = 1; j <= 3; ++j)
	{
		this->normal_vertices.push_back(VertexAttributesP(this->vertices[this->vertices.size() - j].position));
		this->normal_vertices.push_back(VertexAttributesP(this->vertices[this->vertices.size() - j].position + this->vertices[this->vertices.size() - j].normal * normal_scalar));
		this->normal_indices.push_back(this->normal_vertices.size() - 2);
		this->normal_indices.push_back(this->normal_vertices.size() - 1);
	}
}
void Square::SquareVerticies(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, vec3 color)
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
bool Square::Initialize(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
{
	if (this->GLReturnedError("Square::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;
	
	vec3 colori;
	if(solidColor)
	{
		colori = color;
	}
	else
	{
		colori = vec3(colors[0]);
	}
	
	SquareVerticies(p1, p2, p3, p4, colori);



	if (!this->PostGLInitialize(&this->vertex_array_handle, &this->vertex_coordinate_handle, this->vertices.size() * sizeof(VertexAttributesPCN), &this->vertices[0]))
		return false;

	/*	The VertexAttributesPCN class stores vertex attributes: position, color and normal in that order.

		Vertex attributes are stored in an interleaved manner aiding speed of vertex processing.
	*/

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
	
	if (this->GLReturnedError("Square::Initialize - on exit"))
		return false;

	return true;
}

void Square::TakeDown()
{
	this->vertices.clear();
	this->shader.TakeDown();
	this->solid_color.TakeDown();
	super::TakeDown();
}

void Square::Draw(const ivec2 & size)
{
	assert(false);
}
void Square::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size)
{
	super::Draw(projection, modelview, size);
}