/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code it directly copied
*/

#include <iostream>
#include "Sphere.h"

using namespace std;
using namespace glm;

Sphere::Sphere() : Object()
{
	color = vec3(MakeColor(255, 69, 0, 1.0f));
}

bool Sphere::Initialize(float radius, int slices, int stacks)
{
	if (this->GLReturnedError("Sphere::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;

	if (slices < 0)
		slices = 1;
	
	float theta = 360.0f/slices;
	float delta = 180.0f/stacks;
	const vec3 y_axis(0.0f, 1.0f, 0.0f);
	const vec3 z_axis(0.0f, 0.0f, 1.0f);
	const vec4 r_vec(radius, 0.0f, 0.0f, 1.0f);
	vec3 n2;
	vec4 n;
	mat4 tmpM2;
	
	mat4 curr_slice_rotation, next_slice_rotation, curr_stack_rotation, next_stack_rotation;
	for( int j=0; j<slices; j++)
	{
		next_slice_rotation = rotate(curr_slice_rotation, theta, y_axis);
		curr_stack_rotation = mat4();	
		next_stack_rotation = rotate(mat4(), -90.0f, z_axis);
		for(int i=0; i<stacks; i++)
		{
			curr_stack_rotation = next_stack_rotation;
			next_stack_rotation = rotate(curr_stack_rotation, delta, z_axis);

			VertexAttributesPCN cur_vertex_top, cur_vertex_bottom , nxt_vertex_top,nxt_vertex_bottom;
			cur_vertex_top.position = vec3((curr_slice_rotation*curr_stack_rotation)*r_vec);	
			cur_vertex_top.normal = normalize(cur_vertex_top.position);
			nxt_vertex_top.position = vec3((next_slice_rotation*curr_stack_rotation)*r_vec);	
			nxt_vertex_top.normal = normalize(nxt_vertex_top.position);

			cur_vertex_bottom.position = vec3((curr_slice_rotation*next_stack_rotation)*r_vec);	
			cur_vertex_bottom.normal = normalize(cur_vertex_bottom.position);
			nxt_vertex_bottom.position = vec3((next_slice_rotation*next_stack_rotation)*r_vec);	
			nxt_vertex_bottom.normal = normalize(nxt_vertex_bottom.position);
			
			cur_vertex_bottom.color = color;
			cur_vertex_top.color = color;
			nxt_vertex_bottom.color = color;
			nxt_vertex_top.color = color;

			this->vertices.push_back(cur_vertex_top);
			this->vertices.push_back(cur_vertex_bottom);
			this->vertices.push_back(nxt_vertex_bottom);
	
			this->vertex_indices.push_back(this->vertices.size() - 3);
			this->vertex_indices.push_back(this->vertices.size() - 1);
			this->vertex_indices.push_back(this->vertices.size() - 2);

			this->BuildNormalVisualizationGeometry();

			this->vertices.push_back(cur_vertex_top);
			this->vertices.push_back(nxt_vertex_bottom);
			this->vertices.push_back(nxt_vertex_top);
	
			this->vertex_indices.push_back(this->vertices.size() - 3);
			this->vertex_indices.push_back(this->vertices.size() - 1);
			this->vertex_indices.push_back(this->vertices.size() - 2);

			this->BuildNormalVisualizationGeometry();
		}
		curr_slice_rotation = next_slice_rotation;
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
	
	if (this->GLReturnedError("Sphere::Initialize - on exit"))
		return false;

	return true;
}

void Sphere::TakeDown()
{
	this->vertices.clear();
	super::TakeDown();
}

void Sphere::Draw(const mat4 & projection, mat4 modelview, const ivec2 &size)
{
	super::Draw(projection, modelview, size);
}