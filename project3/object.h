/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, most of this code is directly copied
*/

#pragma once
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Globals.h"
#include "shader.h"

#include "vertexattributes.h"

class Object
{
public:
	Object();
	glm::vec3 color;

	virtual void TakeDown();
	virtual bool Initialize();
	virtual bool PostGLInitialize(GLuint * vertex_array_handle, GLuint * vertex_coordinate_handle, GLsizeiptr sz, const GLvoid * ptr);

	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size);
	virtual ~Object();

	inline void EnableNormals(bool dn) { this->draw_normals = dn; } 
	inline glm::vec4 MakeColor(int r, int g, int b, float gain = 1.0f) {	return glm::vec4(float(r) * gain / 255.0f, float(g) * gain / 255.0f, float(b) * gain / 255.0f, 1.0f); }

protected:
	bool draw_normals;
	GLuint vertex_coordinate_handle;
	GLuint vertex_array_handle;
	GLuint normal_coordinate_handle;
	GLuint normal_array_handle;


	bool GLReturnedError(char * s);
	std::vector<GLuint> vertex_indices;
	std::vector<VertexAttributesP> normal_vertices;
	std::vector<GLuint> normal_indices;
	std::vector<VertexAttributesPCN> vertices;

	void BuildNormalVisualizationGeometry();
	typedef Object super;

private:
	Shader solid_color;
	void InternalInitialize();
};
