/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, most of this code directly copied
*/

#include <iostream>
#include <assert.h>
#include "object.h"
#include "shader.h"
#include "Globals.h"

using namespace std;
using namespace glm;

Object::Object()
{
	this->InternalInitialize();
}

Object::~Object()
{
	this->TakeDown();
	assert(this->vertex_array_handle == GLuint(-1));
	assert(this->vertex_coordinate_handle == GLuint(-1));
	assert(this->normal_array_handle == GLuint(-1));
	assert(this->normal_coordinate_handle == GLuint(-1));
}

void Object::TakeDown()
{
	this->normal_indices.clear();
	this->normal_vertices.clear();
	this->vertex_indices.clear();

	if (this->vertex_array_handle != GLuint(-1))
		glDeleteVertexArrays(1, &this->vertex_array_handle);

	if (this->vertex_coordinate_handle != GLuint(-1))
		glDeleteBuffers(1, &this->vertex_coordinate_handle);

	if (this->normal_array_handle != GLuint(-1))
		glDeleteVertexArrays(1, &this->normal_array_handle);

	if (this->normal_coordinate_handle != GLuint(-1))
		glDeleteBuffers(1, &this->normal_coordinate_handle);

	this->InternalInitialize();
}

bool Object::PostGLInitialize(GLuint * vertex_array_handle, GLuint * vertex_coordinate_handle, GLsizeiptr sz, const GLvoid * ptr)
{
	glGenVertexArrays(1, vertex_array_handle);
	glBindVertexArray(*vertex_array_handle);
	glGenBuffers(1, vertex_coordinate_handle);
	glBindBuffer(GL_ARRAY_BUFFER, *vertex_coordinate_handle);
	glBufferData(GL_ARRAY_BUFFER, sz, ptr, GL_STATIC_DRAW);
	return !this->GLReturnedError("Object::PostGLInitialize - on exit");
}

bool Object::Initialize()
{
	solid_color.Initialize("solid_shader.vert", "solid_shader.frag");
	this->InternalInitialize();
	this->draw_normals = false;
	return true;
}

void Object::InternalInitialize()
{
	this->vertex_array_handle = this->vertex_coordinate_handle = GLuint(-1);
	this->normal_array_handle = this->normal_coordinate_handle = GLuint(-1);
}

bool Object::GLReturnedError(char * s)
{
	bool return_error = false;
#ifdef _DEBUG
	GLenum glerror;

	while ((glerror = glGetError()) != GL_NO_ERROR)
	{
		return_error = true;
		cerr << s << ": " << gluErrorString(glerror) << endl;
	}
#endif
	return return_error;
}

void Object::BuildNormalVisualizationGeometry()
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

void Object::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size)
{
	if (this->GLReturnedError("Object::Draw - on entry"))
		return;

	mat4 mvp = projection * modelview;
	mat3 nm = inverse(transpose(mat3(modelview)));

	currShader->Use();
	currShader->CommonSetup(glutGet(GLUT_ELAPSED_TIME), value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
	
	glBindVertexArray(this->vertex_array_handle);
	glDrawElements(GL_TRIANGLES , this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);
	glBindVertexArray(0);
	glUseProgram(0);

	if (this->draw_normals)
	{
		solid_color.Use();
		solid_color.CommonSetup(glutGet(GLUT_ELAPSED_TIME), value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
		glBindVertexArray(this->normal_array_handle);
		glDrawElements(GL_LINES , this->normal_indices.size(), GL_UNSIGNED_INT , &this->normal_indices[0]);
		glBindVertexArray(0);
		glUseProgram(0);
	}
	
	glDisable(GL_CULL_FACE);

	if (this->GLReturnedError("Object::Draw - on exit"))
		return;
}