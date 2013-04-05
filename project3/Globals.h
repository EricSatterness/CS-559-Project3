/*
	Code used to specify lighting values, should be passed as variables
	or moved to namespace shader
*/
#pragma once
#include <glm\glm.hpp>
#include <GL/glew.h>
#include "shader.h"

extern Shader* currShader;
struct LightInfo
{
	glm::vec3 lightPos;
	glm::vec3 lightPosEyeCoords;
	int on;
};
extern LightInfo light1, light2, light3;
extern float l3Intensity;