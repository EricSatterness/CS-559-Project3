#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Globals.h"
#include "ballContactListener.h"
#include "shader.h"
#include "Sphere.h"
#include "Cube.h"
#include "Square.h"
#include <list>
#include "Render.h"
#include "wall.h"
#include "player.h"
#include "moshball.h"

using namespace std;
using namespace glm;

float xRot = 0.0f;
float yRot = 0.0f; 
float zoom = 0.1f;

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

#pragma region Variables
int numBalls = 10;			// Number of moshballs to create in the game
unsigned int seed = 0;		// Seed for the sudo-random moshball positions
int ballSlices = 30;		// Number of slices to use when drawing the balls
int ballStacks = 30;		// Number of stacks to use when drawing the balls
float ballRadius = 1.0f;
int32 framePeriod = 16;

/* Pointers */
Shader shader;
vector<Wall *> walls;
Player* player;
vector<Moshball *> moshballs;

/* Box2D Variables */
DebugDraw debugDraw;
float32 speed = 30.0f;

// Prepare for simulation. Typically we use a time step of 1/60 of a
// second (60Hz) and 10 iterations. This provides a high quality simulation
// in most game scenarios.
float32 timeStep = 1.0f / 60.0f;
int32 velocityIterations = 6;
int32 positionIterations = 2;
#pragma endregion

// Take care of taking down and deleting any items
void CloseFunc()
{
	window.window_handle = -1;

	if (player != NULL)
	{
		//player->TakeDown();
		delete player;
	}

	for (int i = 0; i < (int)moshballs.size(); i++)
	{
		if (moshballs[i] != NULL)
		{
			//moshballs[i]->TakeDown();
			delete moshballs[i];
		}

	}
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
	case '=':
	case '+':
		zoom += .001f;
		break;
	case '-':
		zoom -= .001f;
		break;
	case '8':
		xRot += 1;
		break;
	case '6':
		yRot += 1;
		break;
	case '2':
		xRot -= 1;
		break;
	case '4':
		yRot -= 1;
		break;
	case 'd':
		window.debugBox2D = !window.debugBox2D;
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
			player->body->SetLinearVelocity(b2Vec2(-speed, 0.0f));
			break;
		case GLUT_KEY_RIGHT:
			player->body->SetLinearVelocity(b2Vec2(speed, 0.0f));
			break;
		case GLUT_KEY_UP:
			player->body->SetLinearVelocity(b2Vec2(0.0f, speed));
			break;
		case GLUT_KEY_DOWN:
			player->body->SetLinearVelocity(b2Vec2(0.0f, -speed));
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

	currentTime = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;

	// Instruct the world to perform a single step of simulation.
	// It is generally best to keep the time step and iterations fixed.
	world.Step(timeStep, velocityIterations, positionIterations);
	player->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	
	if (!window.debugBox2D)
	{
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.79f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, window.size.x, window.size.y);
		glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);

		mat4 projection_matrix = perspective(45.0f, window.window_aspect, 1.0f, 10.0f);

		mat4 worldModelView = translate(mat4(1.0f), vec3(0.0f, 0.0f, -8.0f));
		worldModelView = rotate(worldModelView, yRot, vec3(0.0f, 1.0f, 0.0f));
		worldModelView = scale(worldModelView, vec3(zoom, zoom, zoom));
		worldModelView = rotate(worldModelView, xRot, vec3(1.0f, 0.0f, 0.0f));
		
		for (int i = 0; i < (int)walls.size(); i++)
		{
			walls[i]->Draw(projection_matrix, worldModelView, window.size);
		}

		player->Draw(projection_matrix, worldModelView, window.size);

		for (int i = 0; i < (int)moshballs.size(); i++)
		{
			moshballs[i]->Draw(projection_matrix, worldModelView, window.size);
		}

		glutSwapBuffers();
	}
	else
	{
		/* Box2D debug mode */
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
		worldModelView = scale(worldModelView, vec3(zoom, zoom, zoom));
		glLoadMatrixf(value_ptr(worldModelView));


		// Draw a square so I can see the origin (just to make position estimation a little easier)
		glColor3f(1,1,1);
		glBegin(GL_QUADS);
			glVertex3f(-0.1f, -0.1f, 0.0f);
			glVertex3f(0.1f, -0.1f, 0.0f);
			glVertex3f(0.1f, 0.1f, 0.0f);
			glVertex3f(-0.1f, 0.1f, 0.0f);
		glEnd();

		world.DrawDebugData();

		glutSwapBuffers();
	}
}

// This is used to control the frame rate (60Hz).
static void Timer(int)
{
	// Check timers of balls
	for (int i = 0; i < (int)moshballs.size(); i++)
	{
		moshballs[i]->CheckTimer(currentTime);
	}

	//glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 0);
}
int main(int argc, char * argv[])
{
	countDownTimerSeconds = 5.0f;

	glutInit(&argc, argv);
	glutInitWindowSize(1024, 1024);
	glutInitWindowPosition(20, 20);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	window.window_handle = glutCreateWindow("Mosh Balls");
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
	
	/* Box2D testing */
	B2_NOT_USED(argc);
	B2_NOT_USED(argv);

	// Initialize the debug draw methods to only draw shapes
	debugDraw.SetFlags(b2Draw::e_shapeBit);
	world.SetDebugDraw(&debugDraw);

	// Add our contact listener to handle collisions between balls
	BallContactListener ballContactListenerInstance;
	world.SetContactListener(&ballContactListenerInstance);

	// NOTES:
	// Heights and widths in box2D are measured from the center of the shape (not end to end)
	// Friction will only apply when objects collide. In order to slow a ball down, we need to apply damping to the velocity. The body has a linear damping function but we may want to implement our own non-linear damping.

	// Scale: 1 meter = 50 feet; Want player area to be 5280sqft
	//float arena_width = 105.6f;
	float arena_width = 50.0f;
	float wall_t = 0.5f;							// Wall thickness
	float wall_l = (arena_width / 2.0f) + wall_t;	// Wall length. Need to add thickness

	Wall* wall1 = new Wall();
	if (!wall1->Initialize(vec3(0.0f, wall_l - wall_t, 0.0f), wall_l, wall_t, 2*wall_t))
	{
		wall1->TakeDown();
		delete wall1;
		return 0;
	}
	Wall* wall2 = new Wall();
	if (!wall2->Initialize(vec3(wall_l - wall_t, 0.0f, 0.0f), wall_t, wall_l, 2*wall_t))
	{
		wall2->TakeDown();
		delete wall2;
		return 0;
	}
	Wall* wall3 = new Wall();
	if (!wall3->Initialize(vec3(0.0f, -wall_l + wall_t, 0.0f), wall_l, wall_t, 2*wall_t))
	{
		wall3->TakeDown();
		delete wall3;
		return 0;
	}
	Wall* wall4 = new Wall();
	if (!wall4->Initialize(vec3(-wall_l + wall_t, 0.0f, 0.0f), wall_t, wall_l, 2*wall_t))
	{
		wall4->TakeDown();
		delete wall4;
		return 0;
	}
	walls.push_back(wall1);
	walls.push_back(wall2);
	walls.push_back(wall3);
	walls.push_back(wall4);
	
	player = new Player();
	if (!player->Initialize(ballRadius, ballSlices, ballStacks))
	{
		player->TakeDown();
		delete player;
		return 0;
	}

	// Variables to set the range of random starting positions. Arena is oriented at (0,0)
	float position_range = arena_width - (ballRadius * 2.0f);
	float position_offset = position_range / 2.0f;
	srand(seed);
	for (int i = 0; i < numBalls; ++i)
	{
		Moshball* m = new Moshball();

		float x = ((float)rand()/(float)RAND_MAX) * position_range - position_offset;
		float y = ((float)rand()/(float)RAND_MAX) * position_range - position_offset;

		if (!m->Initialize(vec3(x, y, 0.0f), ballRadius, ballSlices, ballStacks))
		{
			m->TakeDown();
			delete m;
			return 0;
		}
		moshballs.push_back(m);
	}

	// Use a timer to control the frame rate.
	glutTimerFunc(framePeriod, Timer, 0);


	glutMainLoop();

	return 0;
}