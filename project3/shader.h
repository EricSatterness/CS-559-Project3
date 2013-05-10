/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, much of this code is directly copied
*/

#pragma once
#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader();
	~Shader();
	void TakeDown();
	void Use();
	virtual bool Initialize(char * vertex_shader_file, char * fragment_shader_file);
	virtual void CustomSetup();
	void CommonSetup(const float time, const GLint * size, const GLfloat * projection, const GLfloat * modelview, const GLfloat * mvp, const GLfloat * nm);

	GLuint modelview_matrix_handle;
	GLuint projection_matrix_handle;
	GLuint normal_matrix_handle;
	GLuint light_position_handle;
	GLuint world_modelview_handle;
	GLuint mvp_handle;
	GLuint size_handle;
	GLuint time_handle;
	GLuint seeThrough_handle;
	GLuint l3_percent_handle;

	GLuint Material_ADSS_handle;
	GLuint l1_pos_handle;
	GLuint l2_pos_handle;
	GLuint l3_pos_handle;
	GLuint on1_handle;
	GLuint on2_handle;
	GLuint on3_handle;
	GLuint tex_handle;
	GLuint tex_handle2;
	GLuint hit_handle;
	GLuint color_handle;

	glm::vec3 color;

	glm::vec4 Material_ADSS;
	int seeThrough;
	int hit;
	GLuint vertex_shader_id;
	GLuint fragment_shader_id;
	GLuint program_id;
	bool LoadShader(const char * file_name, GLuint shader_id);
	std::stringstream GetShaderLog(GLuint shader_id);

protected:
	bool GLReturnedError(char * s);
};

class BackgroundShader : public Shader
{
public:
	BackgroundShader();
	virtual bool Initialize(char * vertex_shader_file, char * fragment_shader_file);
	virtual void CustomSetup(glm::vec4 * color_array);

protected:
	GLuint color_array_handle;

private:
	typedef Shader super;
};

namespace ShaderGlobals
{
	extern Shader* currShader;
};