#include "Globals.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <GL/glew.h>

using namespace std;

/*
	Code used to specify lighting values, should be passed as variables
	or moved to namespace shader
*/

Shader* currShader;
LightInfo light1 = {glm::vec3(2.0f, 2.0f, -2.0f), glm::vec3(), 1};
LightInfo light2 = {glm::vec3(-2.0f, 2.0f, 2.0f), glm::vec3(), 0};
LightInfo light3 = {glm::vec3(0.0f, 1.25f, -3.0f), glm::vec3(), 1};
float l3Intensity;

// Use for ball timers
float currentTime = 0.0f;
float countDownTimerSeconds = 0.0f;
int targetsRemaining = 0;

// Define the gravity vector.
b2Vec2 gravity(0.0f, 0.0f);
// Construct a world object, which will hold and simulate the rigid bodies.
b2World world(gravity);

float box2d_scale = 1.0f;


bool CheckGLErrors(const char * caller)
{

	GLenum e;
	bool r = true;
	
	while ((e = glGetError()) != GL_NO_ERROR)
	{
		r = false;
		cout << caller << " " << gluErrorString(e) << endl;
	}
	return r;
}

const char * FramebufferCompletenessError(GLint error_code)
{
	switch (error_code)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		return "GL_FRAMEBUFFER_UNDEFINED";

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";

	case GL_FRAMEBUFFER_COMPLETE:
		return "GL_FRAMEBUFFER_COMPLETE";

	case GL_FRAMEBUFFER_UNSUPPORTED:
		return "GL_FRAMEBUFFER_UNSUPPORTED";

	default:
		return "Unknown frame buffer error";
	}
}
