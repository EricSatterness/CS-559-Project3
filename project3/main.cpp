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
#include "ImageTexture.h"
#include "Skybox.h"
#include "fbo.h"
//#include <ft2build.h>
//#include FT_FREETYPE_H

using namespace std;
using namespace glm;

#define NUMFRAMEBUFFERS 3

float xRot = 0.0f;
float yRot = 0.0f; 
float zoom = 0.05f;
ImageTexture targetTexture, targetNormals;
Skybox skybox;
FrameBufferObject fbo;
Shader phongShader, targetShader, jumbotronShader;
Cube *jumbotronCube;

#pragma region Data Structs
struct WindowData
{
	int window_handle;
	ivec2 size;
	float window_aspect;
	vec2 origin, mouse;
	mat4 projection_matrix, modelview_matrix;
	vector<string> instructions;
	bool wireframe;
	int sceneIndex;
} window;

struct CameraData
{

};
#pragma endregion

#pragma region Variables
int numBalls = 15;			// Number of moshballs to create in the game
unsigned int seed = 0;		// Seed for the sudo-random moshball positions
int ballSlices = 30;		// Number of slices to use when drawing the balls
int ballStacks = 30;		// Number of stacks to use when drawing the balls
float ballRadius = 1.0f;
int32 framePeriod = 16;
// NOTES:
// Heights and widths in box2D are measured from the center of the shape (not end to end)
// Friction will only apply when objects collide. In order to slow a ball down, we need to apply damping to the velocity. The body has a linear damping function but we may want to implement our own non-linear damping.

// Scale: 1 meter = 50 feet; Want player area to be 5280sqft
float arena_width = 105.6f;
//float arena_width = 50.0f;
float wall_t = 0.5f;							// Wall thickness
float wall_l = (arena_width / 2.0f) + wall_t;	// Wall length. Need to add thickness

/* Pointers */
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

// Freetype variables
//FT_Library ftLibrary;
//FT_Face ftFace;      /* handle to face object */
//FT_GlyphSlot ftSlot;
//FT_Matrix ftMatrix;              /* transformation matrix */
//FT_UInt glyph_index;
//FT_Vector pen;                 /* untransformed origin */
//int n;
#pragma endregion

// Take care of taking down and deleting any items
void CloseFunc()
{
	window.window_handle = -1;

	phongShader.TakeDown();
	targetShader.TakeDown();
	jumbotronShader.TakeDown();
	targetNormals.TakeDown();

	targetTexture.TakeDown();
	
	fbo.TakeDown();

	skybox.TakeDown();

	if (player != NULL)
	{
		player->TakeDown();
		delete player;
	}

	for (int i = 0; i < (int)moshballs.size(); i++)
	{
		if (moshballs[i] != NULL)
		{
			moshballs[i]->TakeDown();
			delete moshballs[i];
		}

	}

	for (int i = 0; i < (int)walls.size(); i++)
	{
		if (walls[i] != NULL)
		{
			walls[i]->TakeDown();
			delete walls[i];
		}
	}
}

void ReshapeFunc(int w, int h)
{
	if (window.window_handle != -1 &&  h > 0)
	{
		window.size = ivec2(w, h);
		window.window_aspect = float(w) / float(h);

		window.origin = vec2((float)window.size.x / 2.0f, (float)window.size.y / 2.0f);

		fbo.TakeDown();
		if(!fbo.Initialize(window.size, NUMFRAMEBUFFERS))
			return;
	}
}

void KeyboardFunc(unsigned char c, int x, int y)
{
	if (window.window_handle == -1)
		return;

	switch (c)
	{
	case 's':
		skybox.NextTexture();
		break;
	case 'q':
		window.sceneIndex = ++window.sceneIndex % 3;
		break;
	case 'w':
		window.wireframe = !window.wireframe;
		break;
	case 'p':
		if (paused)
		{
			// Will be leaving paused state
			totalTimePaused += (currentTime - timeLastPauseBegan);
		}
		else
		{
			// Will be entering paused state
			timeLastPauseBegan = currentTime;
		}
		paused = !paused;
		break;
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
		// Change the rendered scene
		case GLUT_KEY_F1:
			window.sceneIndex = ++window.sceneIndex % 3;
			break;
	}
}

// Grabs the mouse position whenever it moves
void MouseMoveFunc(int x, int y)
{
	window.mouse = vec2((float)x, (float)y);
}

// Handles the player movement
void mouseControlsRoutine()
{
	// Handle the player's reflection when it collides with something
	b2Vec2 v = player->body->GetLinearVelocity();
	float angle = 0;

	if (player->hit)
	{
		player->hit = false;
		//float tan = atan2(-v.y, v.x);
		angle = 180.0f/3.14f * atan2(-v.y, v.x);
		//float tan = atan2(player->hitVelocity.y, -player->hitVelocity.x);
		//angle = 180.0f/3.14f * tan;
		if (angle < 0)
			angle += 360;

		printf("%4.2f %4.2f %4.2f\n", v.x, v.y, angle);

		player->rotation = angle;
	}

	// X offset rotates the player
	// Y offset moves the player forward/backward
	float dx = window.mouse.x - window.origin.x;
	float dy = window.mouse.y - window.origin.y;
	
	// Values of non-linear functions chosen from experimentation
	float rotationOffset = pow(abs(dx), 2.1f) / 100000.0f;
	if (dx < 0)
		player->rotation = player->rotation - rotationOffset;
	else
		player->rotation = player->rotation + rotationOffset;

	float speed = pow(abs(dy), 1.9f) / 2000.0f;
	mat4 velocityMat = mat4();
	velocityMat = rotate(velocityMat, player->rotation, vec3(0.0f, 0.0f, 1.0f));
	vec4 velocity = velocityMat*vec4(speed, 0.0f, 0.0f, 1.0f);
	
	if (dy < 0)
		player->body->SetLinearVelocity(b2Vec2(velocity.x, -velocity.y));
	else
		player->body->SetLinearVelocity(b2Vec2(-velocity.x, velocity.y));
}

// Draws the instructions. We will take these off once we get the god-view in place
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

// Draws the crosshairs
void DisplayCrosshairs()
{
	if (window.window_handle == -1)
		return;

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
	glTranslated(window.origin.x, window.origin.y, -5.5);
	glScaled(10, 10, 10);
	glLineWidth(2.0);

	glBegin(GL_LINES);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(-1.0f, 0.0f);
		glVertex2f(0.0f, 1.0f);
		glVertex2f(0.0f, -1.0f);
	glEnd();
}

// Adds the text showing the time elapsed and the targets remaining
void DisplayStats()
{
	if (window.window_handle == -1)
		return;

	char time [16], targets [16];
	//sprintf(time, "Time: %.2f", ((paused ? timeLastPauseBegan : currentTime) - totalTimePaused));
	sprintf_s(time, "Time: %.2f", ((paused ? timeLastPauseBegan : currentTime) - totalTimePaused));
	//sprintf(targets, "Targets: %d", targetsRemaining);
	sprintf_s(targets, "Targets: %d", targetsRemaining);


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
	glTranslated(window.size.x - 300, 30, -5.5);
	glScaled(0.085, 0.085, 1.0);
	glLineWidth(1.0);

	glPushMatrix();
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) time);
	glPopMatrix();
	glTranslated(1700, 0, 0);

	glPushMatrix();
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) targets);
	glPopMatrix();
}

void drawScene(mat4 projection_matrix, mat4 worldModelView, bool self, bool jumbos, bool sky)
{
	b2Vec2 pos = player->body->GetPosition();

	if(sky)
		skybox.Draw(projection_matrix, worldModelView, window.size);
	
	currShader = &phongShader;
	for (int i = 0; i < (int)walls.size(); i++)
	{
		walls[i]->Draw(projection_matrix, worldModelView, window.size);
	}
		
	targetTexture.Use();
	currShader = &targetShader;
	for (int i = 0; i < (int)moshballs.size(); i++)
	{
		targetShader.hit = moshballs[i]->displayTimer;
		moshballs[i]->Draw(projection_matrix, worldModelView, window.size);
	}
	if(self)
		player->Draw(projection_matrix, worldModelView, window.size);

	if(jumbos)
	{
		if(fbo.boundIndex == 1)
			fbo.Use(0);
		else
			fbo.Use(1);
		currShader = &jumbotronShader;
		jumbotronCube->Draw(projection_matrix, glm::scale(glm::translate(worldModelView, vec3(-(arena_width / 2.0f), 0.0f, 7.0f)), vec3(.5f, 14.0f, 7.0f)), window.size);
		jumbotronCube->Draw(projection_matrix, scale(translate(rotate(worldModelView, 90.0f, vec3(0.0f, 0.0f, 1.0f)), vec3(-(arena_width / 2.0f), 0.0f, 7.0f)), vec3(.5f, 14.0f, 7.0f)), window.size);
		jumbotronCube->Draw(projection_matrix, scale(translate(rotate(worldModelView, -90.0f, vec3(0.0f, 0.0f, 1.0f)), vec3(-(arena_width / 2.0f), 0.0f, 7.0f)), vec3(.5f, 14.0f, 7.0f)), window.size);
		jumbotronCube->Draw(projection_matrix, scale(translate(rotate(worldModelView, 180.0f, vec3(0.0f, 0.0f, 1.0f)), vec3(-(arena_width / 2.0f), 0.0f, 7.0f)), vec3(.5f, 14.0f, 7.0f)), window.size);
	}
}
void renderFirstPersonScene()
{
	glEnable(GL_DEPTH_TEST);
	//glClearColor(0.79f, 1.0f, 1.0f, 1.0f);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.size.x, window.size.y);
	glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);

	mat4 projection_matrix = perspective(45.0f, window.window_aspect, .01f, 300.0f);

	b2Vec2 pos = player->body->GetPosition();

	mat4 worldModelView = mat4(1.0f);
	// Rotate so that we look at positive Y-axis and make positive Z-axis the up vector (game is played in XY plane)
	worldModelView = rotate(worldModelView, -90.0f, vec3(1.0f, 0.0f, 0.0f));
	// X-axis is considered 0 degrees for the player's rotation. So start by looking at positive X-axis and then rotate from there
	worldModelView = rotate(worldModelView, player->rotation + 90.0f, vec3(0.0f, 0.0f, 1.0f));
	worldModelView = translate(worldModelView, vec3(-pos.x, -pos.y, 0.0f));
			
	skybox.SwitchLight(1);
	drawScene(projection_matrix, scale(translate(worldModelView, vec3(0.0f, 0.0f, -2.0f)), vec3(1.0f, 1.0f, -1.0f)), false, true, true);
	skybox.SwitchLight(0);

	glClear(GL_DEPTH_BUFFER_BIT);

	drawScene(projection_matrix, worldModelView, false, true, true);

}

void renderThirdPersonScene()
{
	glEnable(GL_DEPTH_TEST);
	//glClearColor(0.79f, 1.0f, 1.0f, 1.0f);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.size.x, window.size.y);
	glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);

	mat4 projection_matrix = perspective(45.0f, window.window_aspect, 1.0f, 10.0f);

	mat4 worldModelView = translate(mat4(1.0f), vec3(0.0f, 0.0f, -8.0f));
	worldModelView = rotate(worldModelView, yRot, vec3(0.0f, 1.0f, 0.0f));
	worldModelView = rotate(worldModelView, xRot, vec3(1.0f, 0.0f, 0.0f));
	worldModelView = scale(worldModelView, vec3(zoom, zoom, zoom));
		
	//skybox.Draw(projection_matrix, worldModelView, window.size);

	currShader = &phongShader;
	for (int i = 0; i < (int)walls.size(); i++)
	{
		walls[i]->Draw(projection_matrix, worldModelView, window.size);
	}

	player->Draw(projection_matrix, worldModelView, window.size);

	targetTexture.Use();
	currShader = &targetShader;
	for (int i = 0; i < (int)moshballs.size(); i++)
	{
		targetShader.hit = moshballs[i]->displayTimer;
		moshballs[i]->Draw(projection_matrix, worldModelView, window.size);
	}

	//glutSwapBuffers();
}

void renderBox2dDebugScene()
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

	// Draw player heading vector
	b2Vec2 pos = player->body->GetPosition();
	mat4 m = translate(worldModelView, vec3(pos.x, pos.y, 0.0f));
	m = rotate(m, -(player->rotation), vec3(0.0f, 0.0f, 1.0f));
	glLoadMatrixf(value_ptr(m));
	glColor3f(1,1,1);
	glLineWidth(1.0);
	glBegin(GL_LINES);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(3.0f, 0.0f);
	glEnd();

	//glutSwapBuffers();
}

typedef void (*RenderFunction)();
void DisplayFunc()
{
	if (window.window_handle == -1)
		return;

	currentTime = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;

	if (!paused)
	{
		mouseControlsRoutine();

		// Instruct the world to perform a single step of simulation.
		// It is generally best to keep the time step and iterations fixed.
		world.Step(timeStep, velocityIterations, positionIterations);
		//player->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	}
	
	
	// Determine which scene to render on the screen
	RenderFunction render;
	switch (window.sceneIndex)
	{
	case 0:
		render = renderFirstPersonScene;
		fbo.Bind(1);
		render();
		fbo.Unbind();

		fbo.Bind(0);
		render();
		fbo.Unbind();

		fbo.Bind(1);
		render();
		fbo.Unbind();

		render();
		break;
	case 1:
		render = renderThirdPersonScene;
		render();
		break;
	case 2:
		render = renderBox2dDebugScene;
		render();
		break;
	}
	

	DisplayCrosshairs();
	DisplayStats();
	DisplayInstructions();

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.size.x, window.size.y);
	mat4 projection_matrix = glm::ortho(0.0f, (float)window.size.x/2, 0.0f, (float)window.size.y/2);
	mat4 worldModelView  = translate(mat4(1.0f), vec3( window.size.x/2 - window.size.x/13, window.size.y/2 - window.size.y/13, 0.0f));
	drawScene(projection_matrix, worldModelView, true, false, false);
	
	glutSwapBuffers();

	
	if(targetsRemaining == 0)
		glutLeaveMainLoop();

}

// This is used to control the frame rate (60Hz).
static void Timer(int)
{
	if (!paused)
	{
		float currTimeMinusPauses = currentTime - totalTimePaused;
		// Check timers of balls
		for (int i = 0; i < (int)moshballs.size(); i++)
		{
			moshballs[i]->CheckTimer(currTimeMinusPauses);
		}
	}

	//glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 0);
}
int error()
{
	char c;
	scanf("%c", &c);
	return 0;
}
int main(int argc, char * argv[])
{
	if(argc > 1)
		numBalls = atoi(argv[1]);
	if(argc > 2)
		seed = atoi(argv[2]);

	countDownTimerSeconds = 15.0f;

	glutInit(&argc, argv);
	//glutInitWindowSize(1024, 1024);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(20, 20);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	window.window_handle = glutCreateWindow("Mosh Balls");
	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutPassiveMotionFunc(MouseMoveFunc);
	glutCloseFunc(CloseFunc);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	
	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		return error();
	}

	if(!phongShader.Initialize("phongAds.vert", "phongAds.frag"))
		return error();
	if(!targetShader.Initialize("TargetShader.vert", "TargetShader.frag"))
		return error();
	if(!targetNormals.Initialize("normalEarth.jpg"))
		return error();
	glActiveTexture(GL_TEXTURE1);
	targetNormals.Use();
	glActiveTexture(GL_TEXTURE0);
	if(!targetTexture.Initialize("earth.jpg"))
		return error();
	if(!jumbotronShader.Initialize("JumboTronShader.vert", "JumboTronShader.frag"))
		return error();
	if(!skybox.Initialize(arena_width*2.5f))
		return error();
	if(!fbo.Initialize(ivec2(2,2), NUMFRAMEBUFFERS))
		return error();
	currShader = &phongShader;
	jumbotronCube = new Cube();
	jumbotronCube->color = vec3(.625f, .625f, .859f);
	if(!jumbotronCube->Initialize(1.0f))
		return error();


	window.instructions.push_back("Project 3 - UW-Madison - CS 559");
	window.instructions.push_back("Eric Satterness and Chelsey Denton");
	window.instructions.push_back("");
	window.instructions.push_back("8/2/6/4            Rotate god-view camera");
	window.instructions.push_back("+/-                Zoom god-view camera in/out");
	window.instructions.push_back("W                  Turn on/off wireframe mode");
	window.instructions.push_back("P                  Pause game");
	window.instructions.push_back("F1                 Switch between view modes");
	window.instructions.push_back("X                  Exit");
	


	targetsRemaining = numBalls;

	/* Box2D testing */
	B2_NOT_USED(argc);
	B2_NOT_USED(argv);

	// Initialize the debug draw methods to only draw shapes
	debugDraw.SetFlags(b2Draw::e_shapeBit);
	world.SetDebugDraw(&debugDraw);

	// Add our contact listener to handle collisions between balls
	BallContactListener ballContactListenerInstance;
	world.SetContactListener(&ballContactListenerInstance);


	Wall* wall1 = new Wall();
	if (!wall1->Initialize(vec3(0.0f, wall_l - wall_t, 0.0f), wall_l, wall_t, 2*wall_t))
	{
		wall1->TakeDown();
		delete wall1;
		return error();
	}
	Wall* wall2 = new Wall();
	if (!wall2->Initialize(vec3(wall_l - wall_t, 0.0f, 0.0f), wall_t, wall_l, 2*wall_t))
	{
		wall2->TakeDown();
		delete wall2;
		return error();
	}
	Wall* wall3 = new Wall();
	if (!wall3->Initialize(vec3(0.0f, -wall_l + wall_t, 0.0f), wall_l, wall_t, 2*wall_t))
	{
		wall3->TakeDown();
		delete wall3;
		return error();
	}
	Wall* wall4 = new Wall();
	if (!wall4->Initialize(vec3(-wall_l + wall_t, 0.0f, 0.0f), wall_t, wall_l, 2*wall_t))
	{
		wall4->TakeDown();
		delete wall4;
		return error();
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
		return error();
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


	//// Set up freetype
	//int error = FT_Init_FreeType( &ftLibrary );
	//if (error)
	//{
	//	return 0;
 //   }

	//error = FT_New_Face(ftLibrary,
	//					"/usr/share/fonts/truetype/arial.ttf",
	//					0,
	//					&ftFace);
	//if (error == FT_Err_Unknown_File_Format)
	//{
	//	return 0;
	//}
	//else
	//{
	//	return 0;
	//}

	//error = FT_Set_Char_Size(
 //           ftFace,    /* handle to face object           */
 //           0,       /* char_width in 1/64th of points  */
 //           16*64,   /* char_height in 1/64th of points */
 //           300,     /* horizontal device resolution    */
 //           300 );   /* vertical device resolution      */

	//ftSlot = ftFace->glyph;                /* a small shortcut */

	//float angle = 0.0f;
	//char text[] = "hello";
	//int num_chars = 5;
	//int my_target_height = 400;

	///* set up matrix */
	//ftMatrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	//ftMatrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	//ftMatrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	//ftMatrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	///* the pen position in 26.6 cartesian space coordinates */
	///* start at (300,200)                                   */
	//pen.x = 300 * 64;
	//pen.y = ( my_target_height - 200 ) * 64;
	//
	//for ( n = 0; n < num_chars; n++ )
	//{
	//	/* set transformation */
	//	FT_Set_Transform( ftFace, &ftMatrix, &pen );
	//	
	//	/* load glyph image into the slot (erase previous one) */
	//	error = FT_Load_Char( ftFace, text[n], FT_LOAD_RENDER );

	//	if ( error )
	//		continue;  /* ignore errors */

	//	/* now, draw to our target surface (convert position) */
	//	my_draw_bitmap( &ftSlot->bitmap,
	//					ftSlot->bitmap_left,
	//					my_target_height - ftSlot->bitmap_top );

	//	/* increment pen position */
	//	pen.x += ftSlot->advance.x;
	//	pen.y += ftSlot->advance.y;
	//}

	glutMainLoop();

	printf("Game contained %i targets.\n", numBalls);
	printf("Game ends at time: %f.\n", ((paused ? timeLastPauseBegan : currentTime) - totalTimePaused));
	if(targetsRemaining == 0)
		printf("Game has been won.\n");
	else
		printf("Game has been lost.\n");
	printf("Hit enter to exit:");
	char c;
	scanf("%c", &c);

	return 0;
}