#include "Globals.h"
#include <glm/gtc/type_ptr.hpp>

/*
	Code used to specify lighting values, should be passed as variables
	or moved to namespace shader
*/

Shader* currShader;
LightInfo light1 = {glm::vec3(2.0f, 2.0f, -2.0f), glm::vec3(), 1};
LightInfo light2 = {glm::vec3(-2.0f, 2.0f, 2.0f), glm::vec3(), 0};
LightInfo light3 = {glm::vec3(0.0f, 1.25f, -3.0f), glm::vec3(), 1};
float l3Intensity;