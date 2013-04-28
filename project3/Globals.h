/*
	Code used to specify lighting values, should be passed as variables
	or moved to namespace shader
*/
#pragma once
#include <glm\glm.hpp>
#include <GL/glew.h>
#include <Box2D/Box2D.h>
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

bool CheckGLErrors(const char * caller);
const char * FramebufferCompletenessError(GLint error_code);

// Use for ball timers
extern float currentTime;
extern float totalTimePaused;
extern float timeLastPauseBegan;
extern bool paused;
extern float countDownTimerSeconds;
extern int targetsRemaining;

/* Variables for Box2D */
extern b2World world;
extern float box2d_scale;

const int OBJECT_TYPE_PLAYER = 0;
const int OBJECT_TYPE_MOSHBALL = 1;
const int OBJECT_TYPE_WALL = 2;

struct box2dUserData
{
	//bool collidedWithBall;	// True if object is a ball and has collided with the player or another ball
	int objectType;
	void* object;				// Pointer to the object that represents the box2D body
};