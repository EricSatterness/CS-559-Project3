/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code it directly copied
*/

#include <iostream>
#include "Torus.h"

using namespace std;
using namespace glm;

Torus::Torus() : Object()
{
	color = vec3(MakeColor(255, 69, 0, 1.0f));
}

vec3 normalCalc(int stack, float stack_degree_inc, mat4 slice_rotation)
{
	const vec4 n(1.0f, 0.0f, 0.0f, 1.0f);
	return normalize(vec3(rotate(slice_rotation, stack_degree_inc*float(stack)+90.0f, vec3(0.0f, 0.0f, 1.0f))*n));
}
bool Torus::Initialize(float innerRadius, float outerRadius, int nsides, int rings)
{
	if (this->GLReturnedError("Torus::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;

	if (rings <= 0)
		rings = 1;
	
	float theta = 360.0f/rings;
	float delta = 360.0f/nsides;
	const vec3 y_axis(0.0f, 1.0f, 0.0f);
	const vec3 z_axis(0.0f, 0.0f, 1.0f);
	const vec3 r_vec(outerRadius, 0.0f, 0.0f);
	
	mat4 curr_slice_rotation, next_slice_rotation, curr_stack_rotation, tmpM;
	for( int j=0; j<rings; j++)
	{
		vec4 n = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		next_slice_rotation = rotate(curr_slice_rotation, theta, y_axis);
		curr_stack_rotation = mat4();
		for(int i=0; i<nsides; i++)
		{
			VertexAttributesPCN cur_vertex_top, cur_vertex_bottom , nxt_vertex_top,nxt_vertex_bottom;

			tmpM = translate(curr_slice_rotation, r_vec); //rotate for slice and translate out to the radius
			tmpM = tmpM*curr_stack_rotation; //rotate for stack
			cur_vertex_top.position = vec3(tmpM*vec4(0.0f, innerRadius, 0.0f, 1.0f)); //multiplied by radius gives position
			cur_vertex_top.normal = normalCalc(i, delta, curr_slice_rotation);

			tmpM = translate(next_slice_rotation, r_vec);
			tmpM = tmpM*curr_stack_rotation;
			nxt_vertex_top.position = vec3(tmpM*vec4(0.0f, innerRadius, 0.0f, 1.0f));				
			nxt_vertex_top.normal = normalCalc(i, delta, next_slice_rotation);

			curr_stack_rotation = rotate(curr_stack_rotation, delta, z_axis);

			tmpM = translate(curr_slice_rotation, r_vec);
			tmpM = tmpM*curr_stack_rotation;
			cur_vertex_bottom.position = vec3(tmpM*vec4(0.0f, innerRadius, 0.0f, 1.0f));	
			cur_vertex_bottom.normal = normalCalc(i+1, delta, curr_slice_rotation);	
			
			tmpM = translate(next_slice_rotation, r_vec);
			tmpM = tmpM*curr_stack_rotation;
			nxt_vertex_bottom.position = vec3(tmpM*vec4(0.0f, innerRadius, 0.0f, 1.0f));			
			nxt_vertex_bottom.normal = normalCalc(i+1, delta, next_slice_rotation);
			
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
	
	if (this->GLReturnedError("Torus::Initialize - on exit"))
		return false;

	return true;
}

void Torus::TakeDown()
{
	this->vertices.clear();
	super::TakeDown();
}

void Torus::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size)
{
	modelview = rotate(modelview, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	super::Draw(projection, modelview, size);
}
