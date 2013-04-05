/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code it directly copied
*/

#include <iostream>
#include "Spring.h"

using namespace std;
using namespace glm;

Spring::Spring() : Object()
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

void Spring::BuildNormalVisualizationGeometry()
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
vec3 Spring::normalCalc(int i, float delta, mat4 m)
{
	const vec4 n(1.0f, 0.0f, 0.0f, 1.0f);
	return normalize(vec3(rotate(m, delta*float(i)+90.0f, vec3(0.0f, 0.0f, 1.0f))*n));
}
bool Spring::Initialize(float innerRadius, float outerRadius, int nsides, int rings, int loops, float heightPerLoop)
{
	//initialization code same as torus, except increments in z as it rotates around y
	if (this->GLReturnedError("Spring::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;

	if (rings <= 0)
		rings = 1;

	rings *= 4;


	float theta = 360.0f/rings;
	float delta = 360.0f/nsides;
	const vec3 y_axis(0.0f, 1.0f, 0.0f);
	const vec3 z_axis(0.0f, 0.0f, 1.0f);
	const vec3 r_vec(outerRadius, 0.0f, 0.0f);

	float heightInc = heightPerLoop/rings;
	float height = 0.0f;
	
	mat4 m, m_nxt, m2, tmpM;
	for(int k=0; k<loops; k++)
	{
		for( int j=0; j<rings; j++)
		{
			vec4 n = vec4(0.0f, 1.0f, 0.0f, 1.0f);
			m_nxt = rotate(m, theta, y_axis);
			//m_nxt = translate(m, vec3(0.0f, 0.0f, heightInc));
			for(int i=0; i<nsides; i++)
			{
				VertexAttributesPCN cur_vertex_top, cur_vertex_bottom , nxt_vertex_top,nxt_vertex_bottom;

				tmpM = translate(m, r_vec);
				tmpM = tmpM*m2;
				cur_vertex_top.position = vec3(tmpM*vec4(0.0f, innerRadius, 0.0f, 1.0f));
				cur_vertex_top.position.y += height;
				cur_vertex_top.normal = normalCalc(i, delta, m);

				tmpM = translate(m_nxt, r_vec);
				tmpM = tmpM*m2;
				nxt_vertex_top.position = vec3(tmpM*vec4(0.0f, innerRadius, 0.0f, 1.0f));	
				nxt_vertex_top.position.y += height + heightInc;
				nxt_vertex_top.normal = normalCalc(i, delta, m_nxt);

				m2 = rotate(m2, delta, z_axis);

				tmpM = translate(m, r_vec);
				tmpM = tmpM*m2;
				cur_vertex_bottom.position = vec3(tmpM*vec4(0.0f, innerRadius, 0.0f, 1.0f));	
				cur_vertex_bottom.position.y += height;
				cur_vertex_bottom.normal = normalCalc(i+1, delta, m);	
			
				tmpM = translate(m_nxt, r_vec);
				tmpM = tmpM*m2;
				nxt_vertex_bottom.position = vec3(tmpM*vec4(0.0f, innerRadius, 0.0f, 1.0f));	
				nxt_vertex_bottom.position.y += height + heightInc;
				nxt_vertex_bottom.normal = normalCalc(i+1, delta, m_nxt);


				if(solidColor)
				{
					cur_vertex_bottom.color = color;
					cur_vertex_top.color = color;
					nxt_vertex_bottom.color = color;
					nxt_vertex_top.color = color;
				}
				else
				{
					cur_vertex_bottom.color = vec3(this->colors[ColorIndex(j, rings)]);
					cur_vertex_top.color = vec3(this->colors[ColorIndex(j, rings)]);
					nxt_vertex_bottom.color = vec3(this->colors[ColorIndex(j, rings)]);
					nxt_vertex_top.color = vec3(this->colors[ColorIndex(j, rings)]);
				}

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
			height += heightInc;
			m = m_nxt;
		}
	}



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
	
	if (this->GLReturnedError("Spring::Initialize - on exit"))
		return false;

	return true;
}

void Spring::TakeDown()
{
	this->vertices.clear();
	this->shader.TakeDown();
	this->solid_color.TakeDown();
	super::TakeDown();
}

void Spring::Draw(const ivec2 & size)
{
	assert(false);
}

/*	A note about drawing the normals.

	If you scale this object non-uniformly, drawing the normals will
	not be correct. This is because the normals are being stored for
	visualization as geometry. As such, scaling will not be corrected
	by the normal matrix.
*/

/*	A note about the index arrays.

	In this example, the index arrays are unsigned ints. If you know
	for certain that the number of vertices will be small enough, you
	can change the index array type to shorts or bytes. This will have
	the two fold benefit of using less storage and transferring fewer
	bytes.
*/
void Spring::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size)
{
	modelview = rotate(modelview, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	super::Draw(projection, modelview, size);
}
//void Spring::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size)
//{
//	if (this->GLReturnedError("Spring::Draw - on entry"))
//		return;
//
//	glEnable(GL_DEPTH_TEST);
//
//	modelview = rotate(modelview, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
//
//	mat4 mvp = projection * modelview;
//	mat3 nm = inverse(transpose(mat3(modelview)));
//
//	shader.Use();
//	shader.CommonSetup(0.0f, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm), value_ptr(worldModelView));
//	glBindVertexArray(this->vertex_array_handle);
//	glDrawElements(GL_TRIANGLES , this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);
//	glBindVertexArray(0);
//	glUseProgram(0);
//
//	if (this->draw_normals)
//	{
//		this->solid_color.Use();
//		this->solid_color.CommonSetup(0.0f, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm), value_ptr(worldModelView));
//		glBindVertexArray(this->normal_array_handle);
//		glDrawElements(GL_LINES , this->normal_indices.size(), GL_UNSIGNED_INT , &this->normal_indices[0]);
//		glBindVertexArray(0);
//		glUseProgram(0);
//	}
//
//	if (this->GLReturnedError("Top::Draw - on exit"))
//		return;
//}
