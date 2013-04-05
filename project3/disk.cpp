/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code and is directly copied
*/

#include <iostream>
#include "disk.h"

using namespace std;
using namespace glm;

Disk::Disk() : Object()
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

void Disk::BuildNormalVisualizationGeometry()
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

bool Disk::Initialize(float innerRadius, float outerRadius, int slices, int loops)
{
	if (this->GLReturnedError("Disk::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;

	if (slices <= 0)
		slices = 1;

	slices *= 4;

	mat4 m;

	const vec3 n = vec3(0.0f, 1.0f, 0.0f);
	vec4 curr_radius(outerRadius, 0.0f, 0.0f, 1.0f);
	vec4 next_radius(outerRadius, 0.0f, 0.0f, 1.0f);
	const vec3 y_axis(0.0f, 1.0f, 0.0f);
	float x_inc = (innerRadius-outerRadius)/loops;

	const float increment =  360.0f / float(slices);

	for (int j=0; j<loops; ++j)
	{
		curr_radius = next_radius;
		next_radius.x += x_inc;

		for (int i = 0; i < slices; ++i)
		{
			VertexAttributesPCN curr_radius_curr_slices, curr_radius_next_slices , next_radius_curr_slices,next_radius_next_slices;
			curr_radius_next_slices.position = vec3(m * next_radius);
			curr_radius_curr_slices.position = vec3(m * curr_radius);
			
			m = rotate(m, increment, y_axis);
		
			next_radius_next_slices.position = vec3(m * next_radius);
			next_radius_curr_slices.position = vec3(m * curr_radius);
			
			curr_radius_next_slices.normal = n;
			curr_radius_curr_slices.normal = n;
			next_radius_next_slices.normal = n;
			next_radius_curr_slices.normal = n;

			if(solidColor)
			{
				curr_radius_next_slices.color = color;
				curr_radius_curr_slices.color = color;
				next_radius_next_slices.color = color;
				next_radius_curr_slices.color = color;
			}
			else
			{
				curr_radius_next_slices.color = vec3(this->colors[ColorIndex(i, slices)]);
				curr_radius_curr_slices.color = vec3(this->colors[ColorIndex(i, slices)]);
				next_radius_next_slices.color = vec3(this->colors[ColorIndex(i, slices)]);
				next_radius_curr_slices.color = vec3(this->colors[ColorIndex(i, slices)]);
			}
		
		
			this->vertices.push_back(curr_radius_curr_slices);
			this->vertices.push_back(curr_radius_next_slices);
			this->vertices.push_back(next_radius_next_slices);
	
			this->vertex_indices.push_back(this->vertices.size() - 3);
			this->vertex_indices.push_back(this->vertices.size() - 1);
			this->vertex_indices.push_back(this->vertices.size() - 2);

			this->BuildNormalVisualizationGeometry();

			this->vertices.push_back(curr_radius_curr_slices);
			this->vertices.push_back(next_radius_next_slices);
			this->vertices.push_back(next_radius_curr_slices);
	
			this->vertex_indices.push_back(this->vertices.size() - 3);
			this->vertex_indices.push_back(this->vertices.size() - 1);
			this->vertex_indices.push_back(this->vertices.size() - 2);

			this->BuildNormalVisualizationGeometry();
		}
	}


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
	
	if (this->GLReturnedError("Disk::Initialize - on exit"))
		return false;

	return true;
}

void Disk::TakeDown()
{
	this->vertices.clear();
	this->shader.TakeDown();
	this->solid_color.TakeDown();
	super::TakeDown();
}

void Disk::Draw(const ivec2 & size)
{
	assert(false);
}
void Disk::Draw(const mat4 & projection, mat4 modelview, const ivec2 &size)
{
	modelview = rotate(modelview, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	super::Draw(projection, modelview, size);
}