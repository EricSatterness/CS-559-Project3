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
#include "enemy.h"
#include "moshball.h"
#include "ImageTexture.h"
#include "Skybox.h"
#include "fbo.h"
#include "FreeType.h"
#include "ImageWriter.h"
#include <MMSystem.h>
#include <iostream>
#include <Windows.h>

using namespace std;
using namespace glm;

#define NUMFRAMEBUFFERS 3

// OpenAL
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

float xRot = 0.0f;
float yRot = 0.0f; 
float zoom = 0.05f;
ImageTexture targetTexture, targetNormals;
Skybox skybox;
FrameBufferObject fbo;
Shader phongShader, targetShader, jumbotronShader, dynamicTarget, solidShader, dynamicColor, brickShader;
Cube *jumbotronCube;
//ImageWriter* recorder = NULL;

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
	bool displayVelocityVecs;
	int sceneIndex;
} window;
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
Player *player;
Enemy *enemy;
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

const int TICKS_PER_SECOND = 50;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 10;
float next_game_tick;
int loops;

#pragma endregion

#pragma region FreeType
// This holds all the information for the font that we are going to create.
freetype::font_data our_font;	

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

GLuint	base;				// Base Display List For The Font Set
GLfloat	cnt1;				// 1st Counter Used To Move Text & For Coloring
GLfloat	cnt2;				// 2nd Counter Used To Move Text & For Coloring

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	base = glGenLists(96);								// Storage For 96 Characters

	font = CreateFont(	-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						L"Times New Roman");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}

GLvoid KillFont(GLvoid)									// Delete The Font List
{
	glDeleteLists(base, 96);							// Delete All 96 Characters
}

GLvoid glPrint(const char *fmt, ...)					// Custom GL "Print" Routine
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf_s(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}

#pragma endregion
// Take care of taking down and deleting any items
void CloseFunc()
{
	window.window_handle = -1;

	KillFont();
	our_font.clean();

	/*if(recorder != NULL)
	{
		recorder->EndRecording();
		delete recorder;
	}*/

	dynamicColor.TakeDown();
	solidShader.TakeDown();
	dynamicTarget.TakeDown();
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

	if (enemy != NULL)
	{
		enemy->TakeDown();
		delete enemy;
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

		/*if(recorder != NULL)
		{
			recorder->EndRecording();
			delete recorder;
		}
		recorder = new ImageWriter(w, h);
		if(!recorder->InitRecording("TestVid.avi", 20))
			printf("problem");*/

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
	case 'd':
		window.displayVelocityVecs = !window.displayVelocityVecs;
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

		// Check which direction we should rotate the player after bouncing
		if (player->movingForward)
			angle = 180.0f/3.14f * atan2(-v.y, v.x);
		else
			angle = 180.0f/3.14f * atan2(v.y, -v.x);

		// atan2 goes -180 to 180. We have been using 0 to 360 elsewhere in our code
		if (angle < 0)
			angle += 360;

		//printf("%4.2f %4.2f %4.2f\n", v.x, v.y, angle);

		player->rotation = angle;
	}

	// X offset rotates the player
	// Y offset moves the player forward/backward
	float dx = window.mouse.x - window.origin.x;
	float dy = window.mouse.y - window.origin.y;

	if (dy <= 0)
		player->movingForward = true;
	else
		player->movingForward = false;
	
	// Values of non-linear functions chosen from experimentation
	//float rotationOffset = pow(abs(dx), 2.1f) / 100000.0f;
	float rotationOffset = pow(abs(dx), 2.1f) / 200000.0f;
	if (dx < 0)
		player->rotation = player->rotation - rotationOffset;
	else
		player->rotation = player->rotation + rotationOffset;

	//float speed = pow(abs(dy), 1.9f) / 2000.0f;
	float speed = pow(abs(dy), 1.9f) / 5000.0f;
	mat4 velocityMat = mat4();
	velocityMat = rotate(velocityMat, player->rotation, vec3(0.0f, 0.0f, 1.0f));
	vec4 velocity = velocityMat*vec4(speed, 0.0f, 0.0f, 1.0f);
	
	if (dy < 0)
		//player->body->ApplyLinearImpulse(b2Vec2(velocity.x, -velocity.y), b2Vec2(0.0f, 0.0f));
		player->body->SetLinearVelocity(b2Vec2(velocity.x, -velocity.y));
	else
		//player->body->ApplyLinearImpulse(b2Vec2(-velocity.x, velocity.y), b2Vec2(0.0f, 0.0f));
		player->body->SetLinearVelocity(b2Vec2(-velocity.x, velocity.y));
}

void enemyMovementRoutine()
{
	// Handle the player's reflection when it collides with something
	b2Vec2 v = enemy->body->GetLinearVelocity();
	float len = sqrtf(v.x*v.x + v.y*v.y);
	v.x /= len;
	v.y /= len;

	v *= 10.0f;
}

// Draws the instructions. We will take these off once we get the god-view in place
void DisplayInstructions()
{
	if (window.window_handle == -1)
		return;

	vector<string> * s = &window.instructions;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, window.size.x, 0, window.size.y, 1, 10);
	glViewport(0, 0, window.size.x, window.size.y);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float y = 10.0f;
	for (auto i = s->rbegin(); i < s->rend(); ++i)
	{
		freetype::print(true, our_font, 10.0f, y, i->c_str());
		y += 1.2f*our_font.h;
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
	
	char time [16];
	sprintf_s(time, "Time: %.2f", ((paused ? timeLastPauseBegan : currentTime) - totalTimePaused));
	
	string targets = "Targets: ";
	string max = targets + std::to_string((long double)numBalls);
	targets += std::to_string((long double)targetsRemaining);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, window.size.x, 0, window.size.y, 1, 10);
	glViewport(0, 0, window.size.x, window.size.y);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	freetype::print(true, our_font, window.size.x - (max.length() + 12)*our_font.h, 10, targets.c_str());
	freetype::print(true, our_font, window.size.x - 12*our_font.h, 10, time);
}

void drawScene(mat4 projection_matrix, mat4 worldModelView, bool self, bool jumbos, bool sky)
{
	//uncomment to fix light to position
	//light1.lightPosEyeCoords = glm::vec3(worldModelView * glm::vec4(light1.lightPos,1.0));
	//light2.lightPosEyeCoords = glm::vec3(worldModelView * glm::vec4(light2.lightPos,1.0));
	//light3.lightPosEyeCoords = glm::vec3(worldModelView * glm::vec4(light3.lightPos,1.0));

//	b2Vec2 pos = player->body->GetPosition();
	
	if(sky)
		skybox.Draw(projection_matrix, worldModelView, window.size);
	
	//currShader = &phongShader;
	currShader = &brickShader;
	for (int i = 0; i < (int)walls.size(); i++)
	{
		walls[i]->Draw(projection_matrix, worldModelView, window.size);
	}
	 
	currShader = &phongShader;
	if(self)
		player->Draw(projection_matrix, worldModelView, window.size);

	enemy->Draw(projection_matrix, worldModelView, window.size);

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
	
		//jumbo tron posts
		/*jumbotronCube->Draw(projection_matrix, glm::scale(glm::translate(worldModelView, vec3(-(arena_width / 2.0f), 0.0f, 7.0f)), vec3(.5f, 14.0f, 7.0f)), window.size);
		jumbotronCube->Draw(projection_matrix, scale(translate(rotate(worldModelView, 90.0f, vec3(0.0f, 0.0f, 1.0f)), vec3(-(arena_width / 2.0f), 0.0f, 7.0f)), vec3(.5f, 14.0f, 7.0f)), window.size);
		jumbotronCube->Draw(projection_matrix, scale(translate(rotate(worldModelView, -90.0f, vec3(0.0f, 0.0f, 1.0f)), vec3(-(arena_width / 2.0f), 0.0f, 7.0f)), vec3(.5f, 14.0f, 7.0f)), window.size);
		jumbotronCube->Draw(projection_matrix, scale(translate(rotate(worldModelView, 180.0f, vec3(0.0f, 0.0f, 1.0f)), vec3(-(arena_width / 2.0f), 0.0f, 7.0f)), vec3(.5f, 14.0f, 7.0f)), window.size);
		*/
	}

	targetTexture.Use();
	currShader = &targetShader;
	b2Vec2 playerPos = player->body->GetPosition();
	b2Vec2 ballToPlayerVec;
	float angle = 0.0f;
	for (int i = 0; i < (int)moshballs.size(); i++)
	{
		targetShader.hit = moshballs[i]->displayTimer;
		dynamicTarget.hit = moshballs[i]->displayTimer;		

		// Draw direction vector
		if (window.displayVelocityVecs)
		{
			b2Vec2 velocity = moshballs[i]->body->GetLinearVelocity();
			if (velocity.x != 0 || velocity.y != 0)
			{
				b2Vec2 pos =  moshballs[i]->body->GetPosition();
				mat4 m = translate(worldModelView, vec3(pos.x, pos.y, 0.0f));

				angle = 180.0f/3.14f * atan2(-velocity.y, velocity.x);
				m = rotate(m, -angle, vec3(0.0f, 0.0f, 1.0f));
				glLoadMatrixf(value_ptr(m));

				glColor3f(1,1,1);
				glLineWidth(2.0);
				glBegin(GL_LINES);
					glVertex2f(0.0f, 0.0f);
					glVertex2f(3.0f, 0.0f);
				glEnd();
			}
		}

		// Draw the timer text
		if(moshballs[i]->displayTimer)
		{
			// Offset by the ball's position
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(glm::value_ptr(projection_matrix));
			glMatrixMode(GL_MODELVIEW);
			b2Vec2 pos = moshballs[i]->body->GetPosition();
			mat4 m = translate(worldModelView, vec3(pos.x - 0.5f, pos.y + 0.5f, 2.0f));
			m = rotate(m, 90.0f, vec3(1.0f, 0.0f, 0.0f));

			// Rotate to face player
			b2Vec2 ballPos = moshballs[i]->body->GetPosition();
			ballToPlayerVec = playerPos - ballPos;
			angle = 180.0f/3.14f * atan2(ballToPlayerVec.y, ballToPlayerVec.x);
			m = rotate(m, angle + 90.0f, vec3(0.0f, 1.0f, 0.0f));

			// Make text smaller
			m = scale(m, vec3(0.05f, 0.05f, 0.05f));
			glLoadMatrixf(value_ptr(m));

			freetype::print(false, our_font, 0.0f, 0.0f, std::to_string((long double)moshballs[i]->time + 1).c_str());
		}
		if(i%2 == 0)
			currShader = &dynamicTarget;
		else
			currShader = &targetShader;
		targetTexture.Use();
		moshballs[i]->Draw(projection_matrix, worldModelView, window.size);
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
	
	light1.on = false;
	skybox.SwitchLight(1);
	drawScene(projection_matrix, scale(translate(worldModelView, vec3(0.0f, 0.0f, -2.0f)), vec3(1.0f, 1.0f, -1.0f)), false, true, true);
	
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	light1.on = true;
	skybox.SwitchLight(0);	
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

	//currShader = &phongShader;
	currShader = &brickShader;
	for (int i = 0; i < (int)walls.size(); i++)
	{
		walls[i]->Draw(projection_matrix, worldModelView, window.size);
	}

	currShader = &phongShader;
	player->Draw(projection_matrix, worldModelView, window.size);
	enemy->Draw(projection_matrix, worldModelView, window.size);

	targetTexture.Use();
	currShader = &targetShader;
	float angle = 0.0f;
	for (int i = 0; i < (int)moshballs.size(); i++)
	{
		if(i%2 == 0)
			currShader = &dynamicTarget;
		else
			currShader = &targetShader;
		currShader->hit = moshballs[i]->displayTimer;
		moshballs[i]->Draw(projection_matrix, worldModelView, window.size);

		// Draw direction vector
		if (window.displayVelocityVecs)
		{
			b2Vec2 velocity = moshballs[i]->body->GetLinearVelocity();
			if (velocity.x != 0 || velocity.y != 0)
			{
				b2Vec2 pos =  moshballs[i]->body->GetPosition();
				mat4 m = translate(worldModelView, vec3(pos.x, pos.y, 0.0f));

				angle = 180.0f/3.14f * atan2(-velocity.y, velocity.x);
				m = rotate(m, -angle, vec3(0.0f, 0.0f, 1.0f));
				glLoadMatrixf(value_ptr(m));

				glColor3f(1,1,1);
				glLineWidth(1.0);
				glBegin(GL_LINES);
					glVertex2f(0.0f, 0.0f);
					glVertex2f(3.0f, 0.0f);
				glEnd();
			}
		}
	}

	// Draw player heading vector
	if (window.displayVelocityVecs)
	{
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

	// Draw direction vector
	if (window.displayVelocityVecs)
	{
		float angle = 0.0f;
		for (int i = 0; i < (int)moshballs.size(); i++)
		{
			b2Vec2 velocity = moshballs[i]->body->GetLinearVelocity();
			if (velocity.x != 0 || velocity.y != 0)
			{
				b2Vec2 pos =  moshballs[i]->body->GetPosition();
				mat4 m = translate(worldModelView, vec3(pos.x, pos.y, 0.0f));

				angle = 180.0f/3.14f * atan2(-velocity.y, velocity.x);
				m = rotate(m, -angle, vec3(0.0f, 0.0f, 1.0f));
				glLoadMatrixf(value_ptr(m));

				glColor3f(1,1,1);
				glLineWidth(1.0);
				glBegin(GL_LINES);
					glVertex2f(0.0f, 0.0f);
					glVertex2f(3.0f, 0.0f);
				glEnd();
			}
		}

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
	}

	//glutSwapBuffers();
}

typedef void (*RenderFunction)();
void DisplayFunc()
{
	if (window.window_handle == -1)
		return;

	/*if (!paused)
	{
		mouseControlsRoutine();
		enemyMovementRoutine();
	}*/

	loops = 0;
	// Update game state at a constant rate and update frames as fast as possible
	while (float(glutGet(GLUT_ELAPSED_TIME)) > next_game_tick && loops < MAX_FRAMESKIP)
	{
		if (!paused)
		{
			mouseControlsRoutine();
			enemyMovementRoutine();

			// Instruct the world to perform a single step of simulation.
			// It is generally best to keep the time step and iterations fixed.
			world.Step(timeStep, velocityIterations, positionIterations);
			world.ClearForces();
		}

		next_game_tick += SKIP_TICKS;
		loops ++;
	}
	
	currentTime = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
	
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
	//DisplayInstructions();

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.size.x, window.size.y);
	mat4 projection_matrix = glm::ortho(0.0f, (float)window.size.x/2, 0.0f, (float)window.size.y/2);
	mat4 worldModelView  = translate(mat4(1.0f), vec3( arena_width/2, arena_width/2, 0.0f));
	currShader = &solidShader;
	jumbotronCube->Draw(projection_matrix, scale(translate(worldModelView, vec3(0.0f, 0.0f, -1.5f)), vec3(arena_width, arena_width, 1.0f)), window.size);
	for (int i = 0; i < (int)walls.size(); i++){
		walls[i]->Draw(projection_matrix, worldModelView, window.size);
	}
	for (int i = 0; i < (int)moshballs.size(); i++)
	{
		solidShader.hit = moshballs[i]->displayTimer;
		moshballs[i]->Draw(projection_matrix, worldModelView, window.size);
	}	
	solidShader.hit = 0;
	player->Draw(projection_matrix, worldModelView, window.size);
	enemy->Draw(projection_matrix, worldModelView, window.size);

	
	if (!paused)
	{
		float currTimeMinusPauses = currentTime - totalTimePaused;
		// Check timers of balls
		for (int i = 0; i < (int)moshballs.size(); i++)
		{
			moshballs[i]->CheckTimer(currTimeMinusPauses);
		}
	}
	
	//recorder->RecordImage();
	glutSwapBuffers();
	glutPostRedisplay();
	
	if(targetsRemaining == 0)
		glutLeaveMainLoop();

}
int error()
{
	char c;
	scanf_s("%c", &c);
	return 0;
}

int main(int argc, char * argv[])
{
	if(argc > 1)
		numBalls = atoi(argv[1]);
	if(argc > 2)
		seed = atoi(argv[2]);

	countDownTimerSeconds = 30.0f + numBalls * 2.0f;

	glutInit(&argc, argv);
	//glutInitWindowSize(1024, 1024);
	//glutInitWindowSize(800, 600);
	glutInitWindowSize(1200, 950);
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


	if(!brickShader.Initialize("brick_shader.vert", "brick_shader.frag"))
		return error();
	if(!dynamicColor.Initialize("DynamicColor.vert", "DynamicColor.frag"))
		return error();
	if(!solidShader.Initialize("solid_shader.vert", "solid_shader.frag"))
		return error();
	if(!dynamicTarget.Initialize("DynamicSphere.vert", "DynamicSphere.frag"))
		return error();
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
	jumbotronCube->color = vec3(.5f, 1.0f, 1.0f);
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
	Wall* wall5 = new Wall();
	if (!wall5->Initialize(vec3(26.4f, 0.0f, 0.0f), wall_t, 13.2f, 2*wall_t))
	{
		wall5->TakeDown();
		delete wall5;
		return error();
	}
	Wall* wall6 = new Wall();
	if (!wall6->Initialize(vec3(-26.4f, 0.0f, 0.0f), wall_t, 13.2f, 2*wall_t))
	{
		wall6->TakeDown();
		delete wall6;
		return error();
	}
	walls.push_back(wall1);
	walls.push_back(wall2);
	walls.push_back(wall3);
	walls.push_back(wall4);
	walls.push_back(wall5);
	walls.push_back(wall6);
	
	player = new Player();
	if (!player->Initialize(b2Vec2(0.0f, 0.0f), ballRadius, ballSlices, ballStacks))
	{
		player->TakeDown();
		delete player;
		return error();
	}
	enemy = new Enemy();
	if (!enemy->Initialize(b2Vec2(5.0f, 5.0f), ballRadius, ballSlices, ballStacks))
	{
		enemy->TakeDown();
		delete enemy;
		return error();
	}
	enemy->body->SetLinearVelocity(b2Vec2(-1.0f, 3.0f));

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
	//glutTimerFunc(framePeriod, Timer, 0);

	BuildFont();										// Build The Font
	our_font.init("test.TTF", 16);					    //Build the freetype font
	//our_font.init("/usr/share/fonts/truetype/arial.ttf", 16);


	// Start the ambient music
	//PlaySound(TEXT("./media/Hypnotic Ambiance.wav"),NULL,SND_FILENAME|SND_ASYNC|SND_LOOP);


	glutMainLoop();

	printf("Game contained %i targets.\n", numBalls);
	printf("Game ends at time: %f.\n", ((paused ? timeLastPauseBegan : currentTime) - totalTimePaused));
	if(targetsRemaining == 0)
		printf("Game has been won.\n");
	else
		printf("Game has been lost.\n");
	printf("Hit enter to exit:");
	char c;
	scanf_s("%c", &c);

	return 0;
}