/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code and is directly copied
*/

#include <iostream>
#include "Curtain.h"

using namespace std;
using namespace glm;
#define PI 3.141592654f

Curtain::Curtain() : Object()
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

void Curtain::BuildNormalVisualizationGeometry()
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
void Curtain::Square(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, vec3 color, float r, float ri)
{
	VertexAttributesPCN cur_vertex_top, cur_vertex_bottom , nxt_vertex_top,nxt_vertex_bottom;

	cur_vertex_top.position = p1;
	cur_vertex_bottom.position = p2;
	nxt_vertex_top.position = p4;
	nxt_vertex_bottom.position = p3;

	vec3 n = normalize(vec3(cos(r), 1.0f, 0.0f)); //calculate the normal with the knowing that the square is part of a sine wave
	vec3 ni = normalize(vec3(cos(r), 1.0f, 0.0f));
	cur_vertex_bottom.normal = n;
	cur_vertex_top.normal = n;
	nxt_vertex_bottom.normal = ni;
	nxt_vertex_top.normal = ni;

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
bool Curtain::Initialize(float width, float height, float depth, int waves, int slices, int stacks)
{
	if (this->GLReturnedError("Curtain::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;
	

	float x = -width/2;
	float xinc = width/slices;
	float nxtX = x+xinc;
	float y = 0.0f;
	float yinc = height/stacks;
	float nxtY = yinc;
	for(int i=0; i<slices; i++)
	{
		x = nxtX;
		nxtX+=xinc;
		nxtY = 0.0f;
		for(int j=0; j<stacks; j++)
		{
			y = nxtY;
			nxtY+=yinc;
			float r = x*2*PI*waves/width; //calculate radians for sine
			int tmp = int(r/(2*PI));
			r = r - (2*PI*float(tmp)); //r = r mod 2PI
			float ri = nxtX*2*PI*waves/width;
			tmp = int(ri/(2*PI));
			ri = ri - (2*PI*float(tmp));
			float z = depth*sin(r); //create wave of curtain by modulating the z position with a sine wave
			float zi = depth*sin(ri);
			Square(glm::vec3(x, y, z), glm::vec3(x, nxtY, z), glm::vec3(nxtX, nxtY, zi), glm::vec3(nxtX, y, zi), color, r, ri); 
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

	if (this->GLReturnedError("Curtain::Initialize - on exit"))
		return false;

	return true;
}

void Curtain::TakeDown()
{
	this->vertices.clear();
	this->shader.TakeDown();
	this->solid_color.TakeDown();
	super::TakeDown();
}

void Curtain::Draw(const ivec2 & size)
{
	assert(false);
}

void Curtain::Draw(const mat4 & projection, mat4 modelview, const ivec2 &size)
{
	super::Draw(projection, modelview, size);
}