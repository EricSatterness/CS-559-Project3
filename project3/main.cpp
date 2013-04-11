#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "Sphere.h"

#include "Render.h"

using namespace std;
using namespace glm;

Sphere* sphere;
Shader shader;

#pragma region Data Structs
struct WindowData
{
	int window_handle;
	ivec2 size;
	float window_aspect;
	mat4 projection_matrix, modelview_matrix;
	vector<string> instructions;
	bool wireframe;
	bool debugBox2D;
} window;

struct CameraData
{

} mainCamera;
#pragma endregion

#pragma region Box2D Variables
DebugDraw debugDraw;
int32 framePeriod = 16;
float32 speed = 30.0f;

// Define the gravity vector.
b2Vec2 gravity(0.0f, 0.0f);
// Construct a world object, which will hold and simulate the rigid bodies.
b2World world(gravity);

// Prepare for simulation. Typically we use a time step of 1/60 of a
// second (60Hz) and 10 iterations. This provides a high quality simulation
// in most game scenarios.
float32 timeStep = 1.0f / 60.0f;
int32 velocityIterations = 6;
int32 positionIterations = 2;

b2Body* playerBody;
#pragma endregion

// Take care of taking down and deleting any items
void CloseFunc()
{
	window.window_handle = -1;

	/*for (int i = 0; i < (int)window.objects.size(); i++)
	{
		if (window.objects[i] != NULL)
		{
			window.objects[i]->TakeDown();
			delete window.objects[i];
		}

	}*/
}

void ReshapeFunc(int w, int h)
{
	if (window.window_handle != -1 &&  h > 0)
	{
		window.size = ivec2(w, h);
		window.window_aspect = float(w) / float(h);
	}
}

void KeyboardFunc(unsigned char c, int x, int y)
{
	if (window.window_handle == -1)
		return;

	switch (c)
	{
	case 'd':
		window.debugBox2D = ~window.debugBox2D;
		break;
	case 'x':
	case 27:
		glutLeaveMainLoop();
		return;
	}
}

void SpecialFunc(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
			playerBody->SetLinearVelocity(b2Vec2(-speed, 0.0f));
			break;
		case GLUT_KEY_RIGHT:
			playerBody->SetLinearVelocity(b2Vec2(speed, 0.0f));
			break;
		case GLUT_KEY_UP:
			playerBody->SetLinearVelocity(b2Vec2(0.0f, speed));
			break;
		case GLUT_KEY_DOWN:
			playerBody->SetLinearVelocity(b2Vec2(0.0f, -speed));
			break;
	}
}

void DisplayInstructions()
{
	if (window.window_handle == -1)
		return;

	vector<string> * s = &window.instructions;
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glColor3f(1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, window.size.x, 0, window.size.y, 1, 10);
	glViewport(0, 0, window.size.x, window.size.y);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(10, 15 * s->size(), -5.5);
	glScaled(0.1, 0.1, 1.0);
	glLineWidth(1.0);
	for (auto i = s->begin(); i < s->end(); ++i)
	{
		glPushMatrix();
		glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) (*i).c_str());
		glPopMatrix();
		glTranslated(0, -150, 0);
	}
}

void DisplayFunc()
{
	if (window.window_handle == -1)
		return;

	if (!window.debugBox2D)
	{
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.79f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, window.size.x, window.size.y);
		glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);

		glm::mat4 projection_matrix = perspective(45.0f, window.window_aspect, 1.0f, 10.0f);
		glm::mat4 worldModelView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));


		sphere->Draw(projection_matrix, worldModelView, window.size);

		glutSwapBuffers();
		//glutPostRedisplay();
	}
	else
	{
		/* Box2D Testing */
		if (window.window_handle == -1)
			return;

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, window.size.x, window.size.y);
		glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);

		glMatrixMode(GL_PROJECTION);
		mat4 projection_matrix = perspective(45.0f, window.window_aspect, 1.0f, 10.0f);
		glLoadMatrixf(value_ptr(projection_matrix));

		glMatrixMode(GL_MODELVIEW);
		mat4 worldModelView = translate(mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.0f));
		worldModelView = scale(worldModelView, vec3(0.1f, 0.1f, 0.1f));
		//worldModelView = scale(worldModelView, vec3(2.0f, 2.0f, 2.0f));
		glLoadMatrixf(value_ptr(worldModelView));


		glColor3f(1,1,1);
		glBegin(GL_QUADS);
			glVertex3f(-0.1f, -0.1f, 0.0f);
			glVertex3f(0.1f, -0.1f, 0.0f);
			glVertex3f(0.1f, 0.1f, 0.0f);
			glVertex3f(-0.1f, 0.1f, 0.0f);
		glEnd();

		// HelloWorld.cpp
		// Instruct the world to perform a single step of simulation.
		// It is generally best to keep the time step and iterations fixed.
		world.Step(timeStep, velocityIterations, positionIterations);
		world.DrawDebugData();

		playerBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));

		// Now print the position and angle of the body.
		b2Vec2 position = playerBody->GetPosition();
		float32 angle = playerBody->GetAngle();

		printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);

		glutSwapBuffers();
	}
}

// This is used to control the frame rate (60Hz).
static void Timer(int)
{
	//glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 0);
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 1024);
	glutInitWindowPosition(20, 20);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	window.window_handle = glutCreateWindow("1st Person View");
	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutCloseFunc(CloseFunc);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	
	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		return 0;
	}

	if(!shader.Initialize("phongAds.vert", "phongAds.frag"))
	{
		return 0;
	}
	currShader = &shader;
	sphere = new Sphere();
	if(!sphere->Initialize(1.0f, 30, 30))
	{
		sphere->TakeDown();
	}



	/* Box2D testing */
	B2_NOT_USED(argc);
	B2_NOT_USED(argv);

	debugDraw.SetFlags(b2Draw::e_shapeBit);
	world.SetDebugDraw(&debugDraw);

	// NOTES:
	// Friction will only apply when objects collide. In order to slow a ball down, we need to apply damping to the velocity. The body has a linear damping function but we may want to implement our own non-linear damping.

	// TODO: Move these body instantiations into their own classes because we will want to map them to OpenGL objects

	// Make the walls
	// Scale: 1 meter = 50 feet
	float wall_t = 0.5f;
	// Want player area to be 5280sqft, so add wall thickness to make the lengths correct
	//float wall_l = 105.6f + wall+_t;
	float wall_l = 25.0f + wall_t;

	b2BodyDef wallBodyDef;
	wallBodyDef.position.Set(wall_l - wall_t, 0.0f);
	b2Body* wallBody = world.CreateBody(&wallBodyDef);

	b2PolygonShape wallShape;
	wallShape.SetAsBox(wall_t, wall_l);
	wallBody->CreateFixture(&wallShape, 1.0f);

	wallBodyDef.position.Set(-wall_l + wall_t, 0.0f);
	wallBody = world.CreateBody(&wallBodyDef);
	wallBody->CreateFixture(&wallShape, 1.0f);

	wallBodyDef.position.Set(0.0f, wall_l - wall_t);
	wallBody = world.CreateBody(&wallBodyDef);
	wallShape.SetAsBox(wall_l, wall_t);
	wallBody->CreateFixture(&wallShape, 1.0f);

	wallBodyDef.position.Set(0.0f, -wall_l + wall_t);
	wallBody = world.CreateBody(&wallBodyDef);
	wallBody->CreateFixture(&wallShape, 1.0f);


	// Make the player's circle
	b2BodyDef playerDef;
	playerDef.type = b2_dynamicBody;
	playerDef.position.Set(0.0f,0.0f);
	playerBody = world.CreateBody(&playerDef);

	b2CircleShape playerShape;
	playerShape.m_radius = 1.0f;

	b2FixtureDef playerFixture;
	playerFixture.shape = &playerShape;
	playerFixture.density = 1.0f;
	playerFixture.friction = 0.3f;
	playerFixture.restitution = 0.2f;
	playerBody->CreateFixture(&playerFixture);


	// Make a moshball
	b2BodyDef ballDef;
	ballDef.type = b2_dynamicBody;
	ballDef.position.Set(5.0f,5.0f);
	b2Body* ballBody = world.CreateBody(&ballDef);

	b2CircleShape ballShape;
	ballShape.m_radius = 1.0f;

	b2FixtureDef ballFixture;
	ballFixture.shape = &playerShape;
	ballFixture.density = 1.0f;
	ballFixture.friction = 0.3f;
	ballFixture.restitution = 0.5f;
	ballBody->CreateFixture(&ballFixture);
	ballBody->SetLinearDamping(0.25f);

	// Use a timer to control the frame rate.
	glutTimerFunc(framePeriod, Timer, 0);


	glutMainLoop();

	return 0;
}